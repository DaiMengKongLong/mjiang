//
// MessageHandler.h
// 消息处理器：解析客户端发送的 JSON 消息并执行相应操作
//

#ifndef MESSAGE_HANDLER_H
#define MESSAGE_HANDLER_H

#include <string>
#include <memory>
#include <functional>
#include <map>
#include <mutex>
#include "JsonHelper.h"

class Room;
class WebSocketServer;

class MessageHandler {
public:
    explicit MessageHandler(WebSocketServer* server);
    
    // 处理客户端发送的消息
    void handleMessage(int clientFd, const std::string& jsonText);
    
    // 设置房间管理回调
    void setRoomManager(std::function<std::shared_ptr<Room>(const std::string& roomId)> getOrCreateRoom);
    
    // 清理客户端信息（当客户端断开时调用）
    void cleanupClient(int clientFd);
    
private:
    WebSocketServer* server_;
    std::function<std::shared_ptr<Room>(const std::string&)> getOrCreateRoom_;
    
    // 存储客户端与房间/玩家的映射（线程安全）
    struct ClientInfo {
        std::shared_ptr<Room> room;
        std::string playerId;
        std::string nickname;
        int seat;
    };
    std::map<int, ClientInfo> clients_;
    std::mutex clientsMutex_;  // 保护 clients_ 的访问
    
    // 消息处理函数
    void handleJoinRoom(int clientFd, const std::string& jsonText);
    void handlePlayCard(int clientFd, const std::string& jsonText);
    void handleChooseAction(int clientFd, const std::string& jsonText);
    
    // 发送响应消息
    void sendRoomInfo(int clientFd, std::shared_ptr<Room> room);
    void sendRoomInfoToAll(std::shared_ptr<Room> room);
    void sendError(int clientFd, const std::string& code, const std::string& message);
};

#endif // MESSAGE_HANDLER_H
