//
// WebSocketServer.h
// 简单的 WebSocket 服务器实现（仅支持文本帧）
//
// 说明：
// 这是一个最小化的 WebSocket 服务器实现，用于支持客户端使用 WebSocket 连接。
// 实现了基本的 HTTP 握手升级和 WebSocket 文本帧的编码/解码。
//

#ifndef WEBSOCKET_SERVER_H
#define WEBSOCKET_SERVER_H

#include <string>
#include <functional>
#include <thread>
#include <mutex>
#include <map>
#include <atomic>

class WebSocketServer {
public:
    // 消息回调：收到客户端消息时调用
    std::function<void(int clientFd, const std::string& message)> onMessage;
    
    // 连接回调：客户端连接时调用
    std::function<void(int clientFd)> onConnect;
    
    // 断开回调：客户端断开时调用
    std::function<void(int clientFd)> onDisconnect;

    // 启动服务器，监听指定端口
    bool start(int port);
    
    // 停止服务器
    void stop();
    
    // 向指定客户端发送文本消息
    bool sendText(int clientFd, const std::string& text);
    
    // 处理事件循环（阻塞调用）
    void run();

private:
    int listenFd_;
    int port_;
    std::atomic<bool> running_;
    
    // 客户端线程管理
    std::map<int, std::thread> clientThreads_;
    std::mutex threadsMutex_;
    
    // 处理单个客户端的消息循环（在独立线程中运行）
    void handleClient(int clientFd);
    
    // 处理 HTTP 握手升级为 WebSocket
    bool handleHandshake(int clientFd);
    
    // 读取并解析 WebSocket 帧
    bool readFrame(int clientFd, std::string& outMessage);
    
    // 发送 WebSocket 文本帧（线程安全）
    bool sendFrame(int clientFd, const std::string& text);
    
    // 生成 WebSocket 握手响应
    std::string generateHandshakeResponse(const std::string& key);
    
    // SHA1 哈希（用于握手）
    std::string sha1(const std::string& input);
    
    // Base64 编码（用于握手）
    std::string base64Encode(const std::string& input);
};

#endif // WEBSOCKET_SERVER_H
