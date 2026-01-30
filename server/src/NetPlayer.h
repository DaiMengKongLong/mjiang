// NetPlayer.h
// 服务器侧的“网络玩家代理”占位类，后续将实现 IPlayer / IGameEngineEventListener。

#pragma once

#include <memory>
#include <string>

class NetPlayer {
public:
    explicit NetPlayer(const std::string& playerId)
        : playerId_(playerId)
        , seat_(-1) {}

    const std::string& getPlayerId() const { return playerId_; }
    
    void setNickname(const std::string& nickname) { nickname_ = nickname; }
    const std::string& getNickname() const { return nickname_; }

    void setSeat(int seat) { seat_ = seat; }
    int getSeat() const { return seat_; }

    // TODO: 后续增加发送 JSON 到客户端的接口，如：
    // void sendJson(const Json::Value& msg);

private:
    std::string playerId_;
    std::string nickname_;
    int seat_;
    // TODO: 持有网络连接句柄（如 WebSocket/TCP 对象），以便发送消息。
};

