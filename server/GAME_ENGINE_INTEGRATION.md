# 麻将逻辑接入指南

> 本文档说明如何将原项目的 `GameEngine` 和 `GameLogic` 接入到联机服务器中。

---

## 📋 概述

原项目（[xiyoufang/mahjong](https://github.com/xiyoufang/mahjong)）中的核心类：
- `GameEngine`：麻将引擎，控制游戏流程
- `GameLogic`：麻将算法类，处理规则判定
- `IPlayer`：玩家基类接口
- `AIPlayer`：AI 玩家实现
- `RealPlayer`：真实玩家实现
- `AIEngine`：AI 决策引擎

**目标**：将这些类集成到服务器中，让服务器负责所有游戏逻辑，客户端只负责显示。

---

## 🔄 架构设计

### 原项目架构（单机）

```
GameLayer (UI)
    ↓
GameEngine (游戏流程控制)
    ↓
RealPlayer (本地玩家) + AIPlayer (AI)
    ↓
GameLogic (规则判定)
```

### 联机架构（目标）

```
客户端 (UI)
    ↓ (WebSocket)
服务器
    ↓
GameEngine (游戏流程控制)
    ↓
NetPlayer (网络玩家) + AIPlayer (AI)
    ↓
GameLogic (规则判定)
```

---

## 📝 接入步骤

### 步骤 1：复制核心类到服务器

将以下文件从原项目复制到 `server/src/`：

```
原项目 Classes/ 目录：
├── GameEngine.h/.cpp
├── GameLogic.h/.cpp
├── IPlayer.h
├── AIPlayer.h/.cpp
├── RealPlayer.h/.cpp
├── AIEngine.h/.cpp
├── GameCmd.h
└── FvMask.h
```

复制到：
```
server/src/
├── game/
│   ├── GameEngine.h/.cpp
│   ├── GameLogic.h/.cpp
│   ├── IPlayer.h
│   ├── AIPlayer.h/.cpp
│   ├── AIEngine.h/.cpp
│   ├── GameCmd.h
│   └── FvMask.h
```

**注意**：`RealPlayer` 不需要复制，因为服务器端使用 `NetPlayer` 替代。

---

### 步骤 2：修改 NetPlayer 实现 IPlayer 接口

更新 `server/src/NetPlayer.h`：

```cpp
#include "game/IPlayer.h"
#include "game/IGameEngineEventListener.h"  // 如果 GameEngine 中有这个接口

class NetPlayer : public IPlayer, public IGameEngineEventListener {
public:
    explicit NetPlayer(const std::string& playerId, int clientFd);
    
    // IPlayer 接口实现
    virtual void onDrawTile(int tile) override;
    virtual void onPlayTile(int tile) override;
    virtual void onAction(const GameAction& action) override;
    
    // IGameEngineEventListener 接口实现
    virtual void onDealCards(const std::vector<int>& cards) override;
    virtual void onAskPlayCard() override;
    virtual void onAskAction(const ActionOptions& options) override;
    virtual void onRoundResult(const RoundResult& result) override;
    
    // 网络相关
    void sendMessage(const std::string& json);
    int getClientFd() const { return clientFd_; }
    
private:
    int clientFd_;  // WebSocket 客户端文件描述符
    WebSocketServer* server_;  // 用于发送消息
};
```

---

### 步骤 3：在 Room 中集成 GameEngine

更新 `server/src/Room.h`：

```cpp
#include "game/GameEngine.h"

class Room {
public:
    // ... 现有方法 ...
    
    // 启动游戏（使用真实的 GameEngine）
    void startGame();
    
    // 获取 GameEngine（用于处理玩家操作）
    GameEngine* getGameEngine() { return gameEngine_.get(); }
    
private:
    std::unique_ptr<GameEngine> gameEngine_;
    // ... 现有成员 ...
};
```

更新 `server/src/Room.cpp`：

```cpp
void Room::startGame() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (state_ != RoomState::WAITING) {
        return;
    }
    
    if (players_.size() < 4) {
        return;
    }
    
    state_ = RoomState::PLAYING;
    
    // 创建 GameEngine
    gameEngine_ = std::make_unique<GameEngine>();
    
    // 将 NetPlayer 注册到 GameEngine
    for (auto& player : players_) {
        // 将 NetPlayer 转换为 IPlayer
        auto iplayer = std::dynamic_pointer_cast<IPlayer>(player);
        if (iplayer) {
            gameEngine_->addPlayer(iplayer);
        }
    }
    
    // 启动游戏
    gameEngine_->startGame();
    
    std::cout << "[Room] 游戏开始: room=" << roomId_ << std::endl;
}
```

---

### 步骤 4：实现 NetPlayer 的事件处理

更新 `server/src/NetPlayer.cpp`：

```cpp
#include "NetPlayer.h"
#include "WebSocketServer.h"
#include "JsonHelper.h"
#include <sstream>

NetPlayer::NetPlayer(const std::string& playerId, int clientFd, WebSocketServer* server)
    : playerId_(playerId)
    , clientFd_(clientFd)
    , server_(server)
    , seat_(-1) {
}

// IGameEngineEventListener 实现
void NetPlayer::onDealCards(const std::vector<int>& cards) {
    // 构建 deal_cards 消息
    std::ostringstream oss;
    oss << R"({"type":"deal_cards","seat":)" << seat_
        << R"(,"handCards":[)";
    
    for (size_t i = 0; i < cards.size(); ++i) {
        if (i > 0) oss << ",";
        oss << cards[i];
    }
    
    oss << "]}";
    sendMessage(oss.str());
}

void NetPlayer::onAskPlayCard() {
    // 构建 your_turn 消息
    std::string msg = R"({"type":"your_turn","allowedActions":["PLAY_CARD"]})";
    sendMessage(msg);
}

void NetPlayer::onAskAction(const ActionOptions& options) {
    // 构建 ask_action 消息
    std::ostringstream oss;
    oss << R"({"type":"ask_action","card":)" << options.card << R"(,"actions":[)";
    
    for (size_t i = 0; i < options.actions.size(); ++i) {
        if (i > 0) oss << ",";
        oss << "\"" << options.actions[i] << "\"";
    }
    
    oss << "]}";
    sendMessage(oss.str());
}

void NetPlayer::onRoundResult(const RoundResult& result) {
    // 构建 round_result 消息
    std::ostringstream oss;
    oss << R"({"type":"round_result","winnerSeat":)" << result.winnerSeat
        << R"(,"scores":[)";
    
    for (size_t i = 0; i < result.scores.size(); ++i) {
        if (i > 0) oss << ",";
        oss << result.scores[i];
    }
    
    oss << "]}";
    sendMessage(oss.str());
}

// IPlayer 接口实现
void NetPlayer::onDrawTile(int tile) {
    // 服务器端不需要实现（由 GameEngine 控制）
}

void NetPlayer::onPlayTile(int tile) {
    // 服务器端不需要实现（由 GameEngine 控制）
}

void NetPlayer::onAction(const GameAction& action) {
    // 服务器端不需要实现（由 GameEngine 控制）
}

void NetPlayer::sendMessage(const std::string& json) {
    if (server_) {
        server_->sendText(clientFd_, json);
    }
}
```

---

### 步骤 5：在 MessageHandler 中处理游戏操作

更新 `server/src/MessageHandler.cpp`：

```cpp
void MessageHandler::handlePlayCard(int clientFd, const std::string& jsonText) {
    std::lock_guard<std::mutex> lock(clientsMutex_);
    auto it = clients_.find(clientFd);
    if (it == clients_.end()) {
        sendError(clientFd, "NOT_IN_ROOM", "玩家未加入房间");
        return;
    }
    
    auto room = it->second.room;
    if (!room || room->getState() != RoomState::PLAYING) {
        sendError(clientFd, "NOT_PLAYING", "房间不在游戏中");
        return;
    }
    
    int card = JsonHelper::getInt(jsonText, "card");
    
    // 获取对应的 NetPlayer
    auto player = room->getPlayerById(it->second.playerId);
    if (!player) {
        sendError(clientFd, "PLAYER_NOT_FOUND", "玩家不存在");
        return;
    }
    
    // 通过 GameEngine 处理出牌
    auto gameEngine = room->getGameEngine();
    if (gameEngine) {
        // 调用 GameEngine 的出牌方法
        // 注意：需要根据 GameEngine 的实际接口调整
        gameEngine->playerPlayCard(player, card);
    }
}

void MessageHandler::handleChooseAction(int clientFd, const std::string& jsonText) {
    std::lock_guard<std::mutex> lock(clientsMutex_);
    auto it = clients_.find(clientFd);
    if (it == clients_.end()) {
        sendError(clientFd, "NOT_IN_ROOM", "玩家未加入房间");
        return;
    }
    
    auto room = it->second.room;
    if (!room || room->getState() != RoomState::PLAYING) {
        sendError(clientFd, "NOT_PLAYING", "房间不在游戏中");
        return;
    }
    
    std::string action = JsonHelper::getString(jsonText, "action");
    int card = JsonHelper::getInt(jsonText, "card");
    
    // 获取对应的 NetPlayer
    auto player = room->getPlayerById(it->second.playerId);
    if (!player) {
        sendError(clientFd, "PLAYER_NOT_FOUND", "玩家不存在");
        return;
    }
    
    // 通过 GameEngine 处理动作选择
    auto gameEngine = room->getGameEngine();
    if (gameEngine) {
        // 调用 GameEngine 的动作处理方法
        // 注意：需要根据 GameEngine 的实际接口调整
        gameEngine->playerChooseAction(player, action, card);
    }
}
```

---

### 步骤 6：更新 Room::startGameMock

将 `Room::startGameMock()` 改为调用 `startGame()`：

```cpp
void Room::startGameMock() {
    startGame();  // 使用真实的 GameEngine
}
```

---

### 步骤 7：处理 AI 玩家

如果房间中有空位，可以自动添加 AI 玩家：

```cpp
void Room::addAIPlayerIfNeeded() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    while (players_.size() < 4) {
        int seat = static_cast<int>(players_.size());
        std::string aiId = "ai_player_" + std::to_string(seat);
        
        auto aiPlayer = std::make_shared<AIPlayer>(aiId);
        aiPlayer->setSeat(seat);
        aiPlayer->setNickname("AI玩家" + std::to_string(seat + 1));
        
        players_.push_back(aiPlayer);
        playersBySeat_[seat] = aiPlayer;
        
        std::cout << "[Room] 添加 AI 玩家: seat=" << seat << std::endl;
    }
}
```

---

## 🔧 需要调整的地方

### 1. 接口适配

原项目的 `GameEngine` 接口可能与我们的设计不完全匹配，需要：

- 检查 `GameEngine` 的构造函数和初始化方法
- 检查 `addPlayer()` 方法的参数类型
- 检查事件回调接口的定义
- 可能需要创建适配器类

### 2. 依赖关系

原项目可能依赖 Cocos2d-x 的某些类，需要：

- 移除或替换 Cocos2d-x 相关的依赖
- 使用标准 C++ 库替代（如 `std::vector` 替代 `cocos2d::Vector`）

### 3. 编译配置

更新 `server/CMakeLists.txt`：

```cmake
# 添加游戏逻辑源文件
set(GAME_SOURCES
    src/game/GameEngine.cpp
    src/game/GameLogic.cpp
    src/game/AIPlayer.cpp
    src/game/AIEngine.cpp
)

target_sources(mahjong_server_ws PRIVATE ${GAME_SOURCES})
```

---

## 📋 检查清单

接入前：
- [ ] 确认原项目的 `GameEngine` 和 `GameLogic` 接口
- [ ] 检查是否有 Cocos2d-x 依赖
- [ ] 准备测试用例

接入中：
- [ ] 复制核心类到服务器
- [ ] 修改 `NetPlayer` 实现 `IPlayer` 接口
- [ ] 在 `Room` 中集成 `GameEngine`
- [ ] 实现事件处理和数据转换
- [ ] 更新 `MessageHandler` 处理游戏操作

接入后：
- [ ] 编译测试
- [ ] 功能测试（发牌、出牌、吃碰杠胡）
- [ ] 多客户端测试
- [ ] AI 玩家测试

---

## 🐛 常见问题

### Q: GameEngine 依赖 Cocos2d-x 怎么办？

**A**: 
1. 检查依赖的具体类（如 `Ref`、`Vector` 等）
2. 使用标准 C++ 替代（如 `std::shared_ptr`、`std::vector`）
3. 或者创建适配器类

### Q: 事件回调接口不匹配？

**A**: 
1. 检查原项目的 `IGameEngineEventListener` 接口定义
2. 可能需要创建适配器，将原接口转换为我们的协议消息
3. 或者修改 `NetPlayer` 的实现

### Q: 编译错误？

**A**: 
1. 检查头文件路径
2. 检查依赖库
3. 查看 `dev_log.md` 中是否有类似问题记录

---

## 📚 参考

- 原项目：[xiyoufang/mahjong](https://github.com/xiyoufang/mahjong)
- 协议文档：`protocol.md`
- 开发日志：`dev_log.md`

---

**注意**：实际接入时，需要根据原项目的具体实现进行调整。本文档提供的是通用方案，具体细节需要根据实际情况修改。
