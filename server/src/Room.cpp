#include "Room.h"
#include "NetPlayer.h"
#include "game/GameEngine.h"

#include <iostream>
#include <algorithm>
#include <memory>

Room::Room(const std::string& id)
    : roomId_(id)
    , state_(RoomState::WAITING) {
}

size_t Room::getPlayerCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return players_.size();
}

std::vector<std::shared_ptr<NetPlayer>> Room::getPlayers() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return players_;
}

bool Room::addPlayer(const std::shared_ptr<NetPlayer>& player) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // 检查房间状态
    if (state_ != RoomState::WAITING) {
        std::cout << "[Room] 房间不在等待状态，无法加入玩家" << std::endl;
        return false;
    }
    
    // 检查房间是否已满
    if (players_.size() >= 4) {
        std::cout << "[Room] 房间已满，无法加入玩家" << std::endl;
        return false;
    }
    
    // 检查玩家是否已存在
    for (const auto& p : players_) {
        if (p->getPlayerId() == player->getPlayerId()) {
            std::cout << "[Room] 玩家已存在: " << player->getPlayerId() << std::endl;
            return false;
        }
    }
    
    // 分配座位号（按加入顺序）
    int seat = static_cast<int>(players_.size());
    player->setSeat(seat);
    
    // 添加到列表
    players_.push_back(player);
    playersBySeat_[seat] = player;
    
    std::cout << "[Room] 玩家加入: room=" << roomId_
              << ", playerId=" << player->getPlayerId()
              << ", seat=" << seat
              << ", current players=" << players_.size() << std::endl;
    
    return true;
}

bool Room::removePlayer(const std::string& playerId) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = std::find_if(players_.begin(), players_.end(),
        [&playerId](const std::shared_ptr<NetPlayer>& p) {
            return p->getPlayerId() == playerId;
        });
    
    if (it == players_.end()) {
        return false;
    }
    
    int seat = (*it)->getSeat();
    players_.erase(it);
    playersBySeat_.erase(seat);
    
    std::cout << "[Room] 玩家离开: room=" << roomId_
              << ", playerId=" << playerId
              << ", seat=" << seat
              << ", remaining players=" << players_.size() << std::endl;
    
    return true;
}

bool Room::removePlayerBySeat(int seat) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = playersBySeat_.find(seat);
    if (it == playersBySeat_.end()) {
        return false;
    }
    
    std::string playerId = it->second->getPlayerId();
    playersBySeat_.erase(it);
    
    players_.erase(
        std::remove_if(players_.begin(), players_.end(),
            [seat](const std::shared_ptr<NetPlayer>& p) {
                return p->getSeat() == seat;
            }),
        players_.end()
    );
    
    std::cout << "[Room] 玩家离开: room=" << roomId_
              << ", seat=" << seat
              << ", remaining players=" << players_.size() << std::endl;
    
    return true;
}

std::shared_ptr<NetPlayer> Room::getPlayerBySeat(int seat) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = playersBySeat_.find(seat);
    if (it != playersBySeat_.end()) {
        return it->second;
    }
    return nullptr;
}

std::shared_ptr<NetPlayer> Room::getPlayerById(const std::string& playerId) const {
    std::lock_guard<std::mutex> lock(mutex_);
    for (const auto& player : players_) {
        if (player->getPlayerId() == playerId) {
            return player;
        }
    }
    return nullptr;
}

void Room::startGame() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (state_ != RoomState::WAITING) {
        std::cout << "[Room] 房间不在等待状态，无法开始游戏" << std::endl;
        return;
    }
    
    if (players_.size() < 4) {
        std::cout << "[Room] 玩家数量不足，无法开始游戏" << std::endl;
        return;
    }
    
    state_ = RoomState::PLAYING;
    std::cout << "[Room] 开始游戏: room=" << roomId_
              << ", players=" << players_.size() << std::endl;
    
#ifdef USE_GAME_ENGINE
    // 创建 GameEngine（不使用单例，每个房间一个实例）
    // 注意：C++11 没有 make_unique，使用 new
    gameEngine_ = std::unique_ptr<GameEngine>(new GameEngine());
    gameEngine_->init();
    
    // 注册玩家到 GameEngine
    for (size_t i = 0; i < players_.size(); i++) {
        auto player = players_[i];
        if (player) {
            // 设置玩家的事件监听器
            player->setGameEngineEventListener(player.get());
            // 注册玩家到 GameEngine
            gameEngine_->onUserEnter(player.get());
        }
    }
    
    // 启动游戏
    if (gameEngine_->onGameStart()) {
        std::cout << "[Room] 游戏启动成功" << std::endl;
    } else {
        std::cout << "[Room] 游戏启动失败" << std::endl;
        state_ = RoomState::WAITING;
        gameEngine_.reset();
    }
#else
    // 未启用 GameEngine，使用模拟版本
    std::cout << "[Room] 使用模拟游戏逻辑" << std::endl;
#endif
}

void Room::startGameMock() {
    // 调用新的 startGame 方法
    startGame();
}

void Room::finishGame() {
    std::lock_guard<std::mutex> lock(mutex_);
    state_ = RoomState::FINISHED;
    std::cout << "[Room] 游戏结束: room=" << roomId_ << std::endl;
    // TODO: 后续可以重置房间状态，允许重新开始游戏
}
