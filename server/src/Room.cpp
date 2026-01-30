#include "Room.h"
#include "NetPlayer.h"

#include <iostream>
#include <algorithm>

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

void Room::startGameMock() {
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
    // TODO: 后续在这里接入真正 GameEngine 的创建与发牌逻辑。
}

void Room::finishGame() {
    std::lock_guard<std::mutex> lock(mutex_);
    state_ = RoomState::FINISHED;
    std::cout << "[Room] 游戏结束: room=" << roomId_ << std::endl;
    // TODO: 后续可以重置房间状态，允许重新开始游戏
}
