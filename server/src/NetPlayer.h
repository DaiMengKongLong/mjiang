// NetPlayer.h
// 网络玩家代理类，实现 IPlayer 和 IGameEngineEventListener 接口

#pragma once

#include <memory>
#include <string>
#include <cstdint>
#include "game/IPlayer.h"
#include "game/GameEngine.h"

class WebSocketServer;

class NetPlayer : public IPlayer, public IGameEngineEventListener {
public:
    explicit NetPlayer(const std::string& playerId, int clientFd, WebSocketServer* server);
    ~NetPlayer() override = default;

    const std::string& getPlayerId() const { return playerId_; }
    
    void setNickname(const std::string& nickname) { nickname_ = nickname; }
    const std::string& getNickname() const { return nickname_; }

    void setSeat(int seat) { 
        seat_ = seat;
        setChairID(static_cast<uint8_t>(seat));
    }
    int getSeat() const { return seat_; }
    
    int getClientFd() const { return clientFd_; }

    // IGameEngineEventListener 接口实现
    void setIPlayer(IPlayer *pIPlayer) override;
    bool onUserEnterEvent(IPlayer *pIPlayer) override;
    bool onGameStartEvent(CMD_S_GameStart GameStart) override;
    bool onSendCardEvent(CMD_S_SendCard SendCard) override;
    bool onOutCardEvent(CMD_S_OutCard OutCard) override;
    bool onOperateNotifyEvent(CMD_S_OperateNotify OperateNotify) override;
    bool onOperateResultEvent(CMD_S_OperateResult OperateResult) override;
    bool onGameEndEvent(CMD_S_GameEnd GameEnd) override;

private:
    std::string playerId_;
    std::string nickname_;
    int seat_;
    int clientFd_;  // WebSocket 客户端文件描述符
    WebSocketServer* server_;  // 用于发送消息
    
    // 发送 JSON 消息到客户端
    void sendJson(const std::string& json);
};
