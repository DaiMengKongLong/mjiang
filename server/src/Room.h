// Room.h
// 房间管理类，支持玩家加入/离开、状态管理等功能。

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <mutex>

class NetPlayer;

enum class RoomState {
    WAITING,    // 等待玩家加入
    PLAYING,    // 游戏中
    FINISHED    // 已结束
};

class Room {
public:
    explicit Room(const std::string& id);

    const std::string& getId() const { return roomId_; }
    RoomState getState() const { return state_; }
    
    // 获取玩家数量（线程安全）
    size_t getPlayerCount() const;
    
    // 获取所有玩家（线程安全，返回副本）
    std::vector<std::shared_ptr<NetPlayer>> getPlayers() const;
    
    // 根据座位号获取玩家
    std::shared_ptr<NetPlayer> getPlayerBySeat(int seat) const;
    
    // 根据 playerId 获取玩家
    std::shared_ptr<NetPlayer> getPlayerById(const std::string& playerId) const;

    // 新玩家加入房间
    bool addPlayer(const std::shared_ptr<NetPlayer>& player);
    
    // 玩家离开房间
    bool removePlayer(const std::string& playerId);
    bool removePlayerBySeat(int seat);

    // 启动一局游戏（后续接入 GameEngine）
    void startGameMock();
    
    // 结束游戏
    void finishGame();

private:
    std::string roomId_;
    RoomState state_;
    std::vector<std::shared_ptr<NetPlayer>> players_;
    std::map<int, std::shared_ptr<NetPlayer>> playersBySeat_;  // 座位号 -> 玩家映射
    mutable std::mutex mutex_;  // 保护房间数据的互斥锁
};

