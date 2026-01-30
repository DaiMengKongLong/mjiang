//
// main_websocket.cpp
// WebSocket 版本的服务器主程序（集成消息处理和房间管理）
//
// 说明：
// 这是支持 WebSocket 协议的服务器版本，可以与 Cocos2d-x 的 WebSocket 客户端直接通信。
// 集成了消息处理器和房间管理功能，能够处理客户端发送的 join_room、play_card、choose_action 等消息。
//

#include "WebSocketServer.h"
#include "MessageHandler.h"
#include "Room.h"
#include <iostream>
#include <memory>
#include <map>

int main() {
    const int kServerPort = 5555;
    
    WebSocketServer server;
    MessageHandler messageHandler(&server);
    
    // 房间管理：存储所有房间
    std::map<std::string, std::shared_ptr<Room>> rooms;
    
    // 设置房间管理器回调
    messageHandler.setRoomManager([&rooms](const std::string& roomId) -> std::shared_ptr<Room> {
        auto it = rooms.find(roomId);
        if (it != rooms.end()) {
            return it->second;
        }
        
        // 创建新房间
        auto room = std::make_shared<Room>(roomId);
        rooms[roomId] = room;
        std::cout << "[main] 创建新房间: " << roomId << std::endl;
        return room;
    });
    
    // 设置连接回调（简化版：连接时不发送消息，等客户端发送 join_room）
    server.onConnect = [](int clientFd) {
        std::cout << "[mahjong_server] 客户端已连接 (fd=" << clientFd << ")" << std::endl;
        std::cout << "[mahjong_server] 等待客户端发送 join_room 消息..." << std::endl;
    };
    
    // 设置消息回调：转发给 MessageHandler
    server.onMessage = [&messageHandler](int clientFd, const std::string& message) {
        messageHandler.handleMessage(clientFd, message);
    };
    
    // 设置断开回调：清理客户端信息
    server.onDisconnect = [&messageHandler](int clientFd) {
        std::cout << "[mahjong_server] 客户端断开连接 (fd=" << clientFd << ")" << std::endl;
        messageHandler.cleanupClient(clientFd);
    };
    
    // 启动服务器
    if (!server.start(kServerPort)) {
        std::cerr << "[mahjong_server] 启动失败" << std::endl;
        return 1;
    }
    
    std::cout << "[mahjong_server] WebSocket 服务器已启动，监听端口 " << kServerPort << std::endl;
    std::cout << "[mahjong_server] 等待客户端连接..." << std::endl;
    
    // 运行事件循环（阻塞）
    server.run();
    
    server.stop();
    return 0;
}
