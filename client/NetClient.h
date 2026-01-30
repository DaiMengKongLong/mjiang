//
// NetClient.h
// 网络客户端封装类（基于 Cocos2d-x WebSocket）
//
// 职责：
// 1. 管理与服务器的单条 WebSocket 连接
// 2. 负责发送/接收 JSON 文本
// 3. 对外暴露回调接口 onMessage
//

#ifndef NET_CLIENT_H
#define NET_CLIENT_H

#include "cocos2d.h"
#include "network/WebSocket.h"
#include <functional>
#include <string>

USING_NS_CC;
using namespace cocos2d::network;

class NetClient : public Ref {
public:
    static NetClient* getInstance();
    
    // 连接到服务器（url 格式：ws://127.0.0.1:5555 或 ws://your-server.com:5555）
    bool connect(const std::string& url);
    
    // 关闭连接
    void close();
    
    // 发送 JSON 文本
    void sendJson(const std::string& jsonText);
    
    // 检查连接状态
    bool isConnected() const { return connected_; }
    
    // 消息回调：收到服务器消息时调用（参数为原始 JSON 文本）
    std::function<void(const std::string&)> onMessage;
    
    // 连接状态回调
    std::function<void()> onConnected;
    std::function<void()> onDisconnected;
    std::function<void(const std::string&)> onError;

private:
    NetClient();
    ~NetClient();
    
    static NetClient* instance_;
    WebSocket* websocket_;
    bool connected_;
    
    // WebSocket 事件回调
    void onOpen(WebSocket* ws);
    void onMessage(WebSocket* ws, const WebSocket::Data& data);
    void onClose(WebSocket* ws);
    void onError(WebSocket* ws, const WebSocket::ErrorCode& error);
};

#endif // NET_CLIENT_H
