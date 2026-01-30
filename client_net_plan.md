## Cocos 客户端联机改造方案（初版）

> 目标：在保持原有 Cocos2d-x 麻将 UI 结构的前提下，引入网络层，使之与服务器使用 `protocol.md` 中定义的协议通信。

---

### 1. 新增核心类设计

#### 1.1 `NetClient`（网络连接封装）

- 职责：
  - 管理与服务器的单条连接（WebSocket/TCP）。
  - 负责发送/接收 JSON 文本。
  - 对外暴露回调接口 `onMessage`。

示例接口（伪代码）：

```cpp
class NetClient : public cocos2d::Ref {
public:
    static NetClient* getInstance();

    bool connect(const std::string& url);
    void close();

    void sendJson(const std::string& jsonText);

    std::function<void(const std::string&)> onMessage; // 原始 JSON 文本
};
```

> 具体实现可基于 Cocos2d-x 的 `network::WebSocket`，或平台自带的 TCP 封装。

#### 1.2 `NetGameController`（游戏网络控制器）

- 职责：
  - 解析来自 `NetClient` 的 JSON 文本，转换为结构化数据。
  - 根据 `type` 字段调用 `GameLayer` 对应的处理函数。
  - 接收 `GameLayer` 的用户操作请求，组装 JSON 并通过 `NetClient` 发送给服务器。

示例接口（伪代码）：

```cpp
class GameLayer; // 前向声明

class NetGameController {
public:
    explicit NetGameController(GameLayer* layer);

    void onRawMessage(const std::string& jsonText);

    // UI -> 服务器
    void sendJoinRoom(const std::string& roomId,
                      const std::string& playerId,
                      const std::string& nickname);
    void sendPlayCard(int card);
    void sendChooseAction(const std::string& action, int card);

private:
    GameLayer* gameLayer_;

    void handleRoomInfo(const Json::Value& msg);
    void handleDealCards(const Json::Value& msg);
    void handleYourTurn(const Json::Value& msg);
    void handlePlayerPlayCard(const Json::Value& msg);
    void handleAskAction(const Json::Value& msg);
    void handleRoundResult(const Json::Value& msg);
};
```

> 解析部分与 `protocol.md` 强绑定，后续协议变更时务必同步调整。

---

### 2. `GameLayer` 的改造要点

原来的 `GameLayer` 可能直接持有本地的 `GameEngine` 与玩家对象；联机版中：

- 移除（或条件编译）本地 `GameEngine` / `RealPlayer` 相关代码；
- 增加对 `NetGameController` 的引用；
- UI 事件改成通过 `NetGameController` 发送网络请求。

伪代码示例：

```cpp
class GameLayer : public cocos2d::Layer {
public:
    virtual bool init() override {
        // ... 原有 UI 初始化 ...

        netController_ = std::make_unique<NetGameController>(this);
        NetClient::getInstance()->onMessage = [this](const std::string& text) {
            netController_->onRawMessage(text);
        };

        return true;
    }

    // ======== 供 NetGameController 调用的回调 ========
    void onRoomInfo(const RoomInfo& info);
    void onDealCards(int seat, const std::vector<int>& cards);
    void onYourTurn();
    void onPlayerPlayCard(int seat, int card);
    void onAskAction(int card, const std::vector<std::string>& actions);
    void onRoundResult(const RoundResult& result);

    // ======== UI 事件回调 ========
    void onClickPlayCard(int card) {
        netController_->sendPlayCard(card);
    }

    void onClickActionPeng(int card) {
        netController_->sendChooseAction("PENG", card);
    }
    // 其他动作类似：GANG / HU / GUO

private:
    std::unique_ptr<NetGameController> netController_;
};
```

---

### 3. 假数据联机流程（调试阶段建议）

在服务器真实接入麻将逻辑之前，建议：

1. 服务器返回固定的 `deal_cards` / `player_play_card` / `ask_action` 消息；
2. 客户端根据这些假数据完成 UI 刷新与按钮交互；
3. 确认：
   - JSON 能正常序列化/反序列化；
   - `GameLayer` 的显示状态与协议字段完全对应；
   - 基础网络断线/重连流程稳定。

> 如果在此阶段遇到解析错误、UI 状态异常等，请在 `dev_log.md` 中记录具体 JSON 报文、出错堆栈与修复方案。

---

### 4. 常见错误与预防

1. **客户端与服务器字段不一致**
   - 现象：客户端解析 JSON 时，某些字段为空或默认值，导致 UI 状态错误。
   - 预防：修改协议时先更新 `protocol.md`，再同步改服务器与客户端；在调试日志中打印原始 JSON。

2. **在 UI 线程外更新 Cocos 节点**
   - 现象：随机崩溃或界面不刷新。
   - 预防：`NetClient` 收到消息后，如非主线程回调，需使用 `Director::getInstance()->getScheduler()->performFunctionInCocosThread` 将 UI 更新投递到主线程。

3. **多处直接操作网络连接**
   - 现象：连接状态混乱、重复发送或资源泄露。
   - 预防：所有网络收发都通过 `NetClient` 单例进行，其他模块只操作 `NetGameController` 与 `GameLayer`。

