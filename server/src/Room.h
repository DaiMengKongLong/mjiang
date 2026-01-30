// Room.h
// 房间管理类，支持玩家加入/离开、状态管理等功能。

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <mutex>

class NetPlayer;

#ifdef USE_GAME_ENGINE
#include "game/GameEngine.h"
#endif

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

    // 启动一局游戏（使用 GameEngine）
    void startGame();
    
    // 启动一局游戏（模拟版本，已废弃）
    void startGameMock();
    
    // 结束游戏
    void finishGame();
    
#ifdef USE_GAME_ENGINE
    // 获取 GameEngine（用于出牌等操作）
    GameEngine* getGameEngine() { return gameEngine_.get(); }
#endif

private:
    std::string roomId_;
    RoomState state_;
    std::vector<std::shared_ptr<NetPlayer>> players_;
    std::map<int, std::shared_ptr<NetPlayer>> playersBySeat_;  // 座位号 -> 玩家映射
    mutable std::mutex mutex_;  // 保护房间数据的互斥锁
#ifdef USE_GAME_ENGINE
    std::unique_ptr<GameEngine> gameEngine_;  // 游戏引擎
#endif
};

