# GameLayer 集成指南

> 本文档说明如何将 `NetClient` 和 `NetGameController` 集成到现有的 `GameLayer` 中。

---

## 1. GameLayer 需要实现的回调接口

`NetGameController` 会调用以下 `GameLayer` 方法，你需要在 `GameLayer.h/.cpp` 中实现它们：

```cpp
// GameLayer.h 中需要添加的接口

#include "NetGameController.h"  // 需要前向声明或包含

class GameLayer : public cocos2d::Layer {
public:
    // ... 原有代码 ...
    
    // ========== 供 NetGameController 调用的回调（必须实现）==========
    
    // 收到房间信息
    void onRoomInfo(const RoomInfo& info);
    
    // 收到发牌消息
    void onDealCards(int seat, const std::vector<int>& cards);
    
    // 轮到自己出牌
    void onYourTurn();
    
    // 某个玩家打出一张牌
    void onPlayerPlayCard(int seat, int card);
    
    // 服务器询问是否要碰/杠/胡/过
    void onAskAction(int card, const std::vector<std::string>& actions);
    
    // 一局结算
    void onRoundResult(const RoundResult& result);
    
private:
    std::unique_ptr<NetGameController> netController_;
};
```

---

## 2. GameLayer::init() 中的初始化代码

在 `GameLayer::init()` 中，添加网络初始化：

```cpp
bool GameLayer::init() {
    if (!Layer::init()) {
        return false;
    }
    
    // ... 原有 UI 初始化代码 ...
    
    // ========== 网络初始化 ==========
    netController_ = std::make_unique<NetGameController>(this);
    
    auto netClient = NetClient::getInstance();
    
    // 设置消息回调
    netClient->onMessage = [this](const std::string& jsonText) {
        // 确保在主线程执行（WebSocket 回调可能在非主线程）
        Director::getInstance()->getScheduler()->performFunctionInCocosThread([this, jsonText]() {
            netController_->onRawMessage(jsonText);
        });
    };
    
    // 设置连接状态回调（可选）
    netClient->onConnected = []() {
        CCLOG("[GameLayer] 已连接到服务器");
    };
    
    netClient->onDisconnected = []() {
        CCLOG("[GameLayer] 与服务器断开连接");
    };
    
    netClient->onError = [](const std::string& errorMsg) {
        CCLOGERROR("[GameLayer] 网络错误: %s", errorMsg.c_str());
    };
    
    // 连接到服务器（示例：本地测试服务器）
    netClient->connect("ws://127.0.0.1:5555");
    
    return true;
}
```

---

## 3. 修改 UI 事件处理（出牌、碰、杠、胡、过）

将原来直接调用本地 `GameEngine` 的代码，改为通过 `NetGameController` 发送网络消息：

```cpp
// 示例：玩家点击出牌按钮
void GameLayer::onClickPlayCard(int card) {
    // 原来可能是：gameEngine_->playCard(card);
    // 现在改为：
    netController_->sendPlayCard(card);
}

// 示例：玩家点击碰按钮
void GameLayer::onClickActionPeng(int card) {
    // 原来可能是：gameEngine_->chooseAction("PENG", card);
    // 现在改为：
    netController_->sendChooseAction("PENG", card);
}

// 类似地，杠、胡、过：
void GameLayer::onClickActionGang(int card) {
    netController_->sendChooseAction("GANG", card);
}

void GameLayer::onClickActionHu(int card) {
    netController_->sendChooseAction("HU", card);
}

void GameLayer::onClickActionGuo() {
    netController_->sendChooseAction("GUO", 0);  // GUO 不需要 card，传 0
}
```

---

## 4. 实现回调函数示例

```cpp
// GameLayer.cpp

void GameLayer::onRoomInfo(const RoomInfo& info) {
    CCLOG("[GameLayer] 房间信息: roomId=%s, players_count=%zu", 
          info.roomId.c_str(), info.players.size());
    
    // 更新 UI：显示房间号、玩家列表等
    // 例如：更新玩家昵称标签
    for (const auto& player : info.players) {
        // 根据 seat 找到对应的 UI 节点，更新昵称
        // updatePlayerNickname(player.seat, player.nickname);
    }
}

void GameLayer::onDealCards(int seat, const std::vector<int>& cards) {
    CCLOG("[GameLayer] 发牌: seat=%d, cards_count=%zu", seat, cards.size());
    
    // 更新 UI：显示手牌
    if (seat == 0) {  // 假设 seat 0 是自己
        // 显示自己的手牌
        // displayHandCards(cards);
    } else {
        // 显示其他玩家的手牌背面（或只显示数量）
        // displayOtherPlayerCards(seat, cards.size());
    }
}

void GameLayer::onYourTurn() {
    CCLOG("[GameLayer] 轮到我出牌");
    
    // 更新 UI：高亮手牌、显示"请出牌"提示、启用出牌按钮等
    // enablePlayCardButtons(true);
    // showHint("请选择一张牌打出");
}

void GameLayer::onPlayerPlayCard(int seat, int card) {
    CCLOG("[GameLayer] 玩家出牌: seat=%d, card=%d", seat, card);
    
    // 更新 UI：在牌河中显示这张牌
    // addCardToRiver(seat, card);
    
    // 如果是我自己出的牌，从手牌中移除
    if (seat == 0) {
        // removeCardFromHand(card);
    }
}

void GameLayer::onAskAction(int card, const std::vector<std::string>& actions) {
    CCLOG("[GameLayer] 询问动作: card=%d, actions_count=%zu", card, actions.size());
    
    // 更新 UI：显示可用的动作按钮（碰、杠、胡、过）
    // showActionButtons(actions);
    // 例如：
    // if (std::find(actions.begin(), actions.end(), "PENG") != actions.end()) {
    //     enablePengButton(true);
    // }
    // ... 其他按钮类似
}

void GameLayer::onRoundResult(const RoundResult& result) {
    CCLOG("[GameLayer] 结算: winnerSeat=%d", result.winnerSeat);
    
    // 更新 UI：显示结算界面
    // showGameOverLayer(result);
    // 例如：显示"玩家X胜利"、各玩家得分等
}
```

---

## 5. 注意事项

1. **移除或条件编译本地 GameEngine 代码**：
   - 如果原 `GameLayer` 中直接持有 `GameEngine` 和 `RealPlayer`，需要移除或使用 `#ifdef` 条件编译。
   - 所有游戏逻辑判断都应在服务器端完成，客户端只负责显示。

2. **JSON 解析库替换**：
   - `NetGameController.cpp` 中的 `JsonHelper` 是简化实现，对嵌套数组支持不完整。
   - 实际集成时，建议使用 rapidjson（Cocos2d-x 通常自带）或 nlohmann/json。
   - 替换方法：在 `NetGameController.cpp` 中，将所有 `JsonHelper::getXXX` 调用替换为 rapidjson 的解析代码。

3. **线程安全**：
   - WebSocket 回调可能在非主线程，所有 UI 更新都应通过 `performFunctionInCocosThread` 投递到主线程。

4. **错误处理**：
   - 网络断开、服务器错误等情况，应在 `onError` 回调中显示提示框，引导用户重连。

5. **测试流程**：
   - 先启动 `mahjong_server`（监听 5555 端口）。
   - 再运行 Cocos 客户端，确认能收到服务器发送的假数据消息。
   - 检查 UI 是否正确刷新。

---

## 6. 快速测试步骤

1. 编译并运行服务器：
```bash
cd server
mkdir build && cd build
cmake ..
make
./mahjong_server
```

2. 在 Cocos 项目中：
   - 将 `client/NetClient.h/.cpp` 和 `client/NetGameController.h/.cpp` 添加到工程。
   - 在 `GameLayer::init()` 中添加网络初始化代码。
   - 实现上述回调函数（可以先只打印日志，不更新 UI）。
   - 运行客户端，观察日志输出，确认能收到服务器的三条测试消息。

3. 如果收到消息但解析失败：
   - 检查 `protocol.md` 中的字段名是否与服务器发送的一致。
   - 检查 JSON 格式是否正确（可以用在线 JSON 验证工具）。
   - 考虑替换为 rapidjson 等专业库。
