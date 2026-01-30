//
// WebSocketServer.cpp
// WebSocket 服务器实现
//

#include "WebSocketServer.h"
#include <iostream>
#include <cstring>
#include <sstream>
#include <algorithm>
#include <openssl/sha.h>
#include <openssl/evp.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

namespace {

// 简单的 Base64 编码实现
std::string base64EncodeSimple(const std::string& input) {
    static const char base64_chars[] = 
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    
    std::string result;
    int val = 0, valb = -6;
    for (unsigned char c : input) {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0) {
            result.push_back(base64_chars[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    if (valb > -6) {
        result.push_back(base64_chars[((val << 8) >> (valb + 8)) & 0x3F]);
    }
    while (result.size() % 4) {
        result.push_back('=');
    }
    return result;
}

// 从 HTTP 请求头中提取 Sec-WebSocket-Key
std::string extractWebSocketKey(const std::string& request) {
    std::istringstream iss(request);
    std::string line;
    while (std::getline(iss, line)) {
        // 去除 \r
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        
        // 查找 Sec-WebSocket-Key 头
        std::string key = "Sec-WebSocket-Key:";
        if (line.size() > key.size() && 
            line.substr(0, key.size()) == key) {
            std::string value = line.substr(key.size());
            // 去除前导空格
            value.erase(0, value.find_first_not_of(" \t"));
            return value;
        }
    }
    return "";
}

} // namespace

bool WebSocketServer::start(int port) {
    port_ = port;
    running_ = false;
    clientThreads_.clear();
    
    listenFd_ = ::socket(AF_INET, SOCK_STREAM, 0);
    if (listenFd_ < 0) {
        std::perror("[WebSocketServer] 创建 socket 失败");
        return false;
    }
    
    int opt = 1;
    ::setsockopt(listenFd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);
    
    if (::bind(listenFd_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        std::perror("[WebSocketServer] 绑定端口失败");
        ::close(listenFd_);
        return false;
    }
    
    if (::listen(listenFd_, 10) < 0) {
        std::perror("[WebSocketServer] listen 失败");
        ::close(listenFd_);
        return false;
    }
    
    running_ = true;
    std::cout << "[WebSocketServer] 启动成功，监听端口 " << port << std::endl;
    return true;
}

void WebSocketServer::stop() {
    running_ = false;
    
    // 等待所有客户端线程结束
    {
        std::lock_guard<std::mutex> lock(threadsMutex_);
        for (auto& pair : clientThreads_) {
            if (pair.second.joinable()) {
                pair.second.join();
            }
        }
        clientThreads_.clear();
    }
    
    if (listenFd_ >= 0) {
        ::close(listenFd_);
        listenFd_ = -1;
    }
}

bool WebSocketServer::handleHandshake(int clientFd) {
    // 读取 HTTP 请求头（最多 4KB）
    char buffer[4096];
    ssize_t n = ::recv(clientFd, buffer, sizeof(buffer) - 1, 0);
    if (n <= 0) {
        return false;
    }
    buffer[n] = '\0';
    
    std::string request(buffer);
    
    // 提取 Sec-WebSocket-Key
    std::string key = extractWebSocketKey(request);
    if (key.empty()) {
        std::cout << "[WebSocketServer] 未找到 Sec-WebSocket-Key，拒绝连接" << std::endl;
        return false;
    }
    
    // 生成响应
    std::string response = generateHandshakeResponse(key);
    
    // 发送响应
    ssize_t sent = ::send(clientFd, response.c_str(), response.size(), 0);
    if (sent < 0) {
        std::perror("[WebSocketServer] 发送握手响应失败");
        return false;
    }
    
    std::cout << "[WebSocketServer] WebSocket 握手成功" << std::endl;
    return true;
}

std::string WebSocketServer::generateHandshakeResponse(const std::string& key) {
    // WebSocket 握手：key + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"
    const std::string magic = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    std::string combined = key + magic;
    
    // SHA1 哈希
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1(reinterpret_cast<const unsigned char*>(combined.c_str()), combined.size(), hash);
    
    // Base64 编码
    std::string accept = base64EncodeSimple(std::string(reinterpret_cast<const char*>(hash), SHA_DIGEST_LENGTH));
    
    // 构造 HTTP 响应
    std::ostringstream oss;
    oss << "HTTP/1.1 101 Switching Protocols\r\n"
        << "Upgrade: websocket\r\n"
        << "Connection: Upgrade\r\n"
        << "Sec-WebSocket-Accept: " << accept << "\r\n"
        << "\r\n";
    
    return oss.str();
}

bool WebSocketServer::readFrame(int clientFd, std::string& outMessage) {
    // 读取帧头（至少 2 字节）
    unsigned char header[2];
    ssize_t n = ::recv(clientFd, header, 2, MSG_WAITALL);
    if (n != 2) {
        return false;
    }
    
    // 解析帧头
    bool fin = (header[0] & 0x80) != 0;
    int opcode = header[0] & 0x0F;
    bool masked = (header[1] & 0x80) != 0;
    uint64_t payloadLen = header[1] & 0x7F;
    
    // 读取扩展长度
    if (payloadLen == 126) {
        uint16_t len16;
        n = ::recv(clientFd, &len16, 2, MSG_WAITALL);
        if (n != 2) return false;
        payloadLen = ntohs(len16);
    } else if (payloadLen == 127) {
        uint64_t len64;
        n = ::recv(clientFd, &len64, 8, MSG_WAITALL);
        if (n != 8) return false;
        // 注意：这里假设是网络字节序，实际需要转换
        payloadLen = len64;
    }
    
    // 读取 mask（客户端必须发送 masked 帧）
    unsigned char mask[4];
    if (masked) {
        n = ::recv(clientFd, mask, 4, MSG_WAITALL);
        if (n != 4) return false;
    }
    
    // 读取 payload
    if (payloadLen > 0) {
        std::vector<char> payload(payloadLen);
        n = ::recv(clientFd, payload.data(), payloadLen, MSG_WAITALL);
        if (n != static_cast<ssize_t>(payloadLen)) return false;
        
        // 解码（应用 mask）
        if (masked) {
            for (size_t i = 0; i < payloadLen; ++i) {
                payload[i] ^= mask[i % 4];
            }
        }
        
        outMessage.assign(payload.data(), payloadLen);
    } else {
        outMessage.clear();
    }
    
    // 只处理文本帧（opcode 1）
    if (opcode == 1) {
        return true;
    } else if (opcode == 8) {
        // 关闭帧
        return false;
    }
    
    return true;
}

bool WebSocketServer::sendFrame(int clientFd, const std::string& text) {
    // 使用互斥锁保护 send 操作（虽然 send 本身是线程安全的，但为了确保帧完整性）
    std::lock_guard<std::mutex> lock(threadsMutex_);
    
    std::vector<unsigned char> frame;
    
    // 帧头：FIN=1, opcode=1 (文本帧)
    frame.push_back(0x81);
    
    // Payload 长度
    size_t len = text.size();
    if (len < 126) {
        frame.push_back(static_cast<unsigned char>(len));
    } else if (len < 65536) {
        frame.push_back(126);
        frame.push_back((len >> 8) & 0xFF);
        frame.push_back(len & 0xFF);
    } else {
        frame.push_back(127);
        for (int i = 7; i >= 0; --i) {
            frame.push_back((len >> (i * 8)) & 0xFF);
        }
    }
    
    // Payload（服务器发送不需要 mask）
    frame.insert(frame.end(), text.begin(), text.end());
    
    ssize_t sent = ::send(clientFd, frame.data(), frame.size(), 0);
    return sent == static_cast<ssize_t>(frame.size());
}

bool WebSocketServer::sendText(int clientFd, const std::string& text) {
    return sendFrame(clientFd, text);
}

void WebSocketServer::handleClient(int clientFd) {
    // 处理消息循环（在独立线程中运行）
    while (running_) {
        std::string message;
        if (!readFrame(clientFd, message)) {
            std::cout << "[WebSocketServer] 客户端断开连接 (fd=" << clientFd << ")" << std::endl;
            break;
        }
        
        // 调用消息回调
        if (onMessage) {
            onMessage(clientFd, message);
        }
    }
    
    // 调用断开回调
    if (onDisconnect) {
        onDisconnect(clientFd);
    }
    
    ::close(clientFd);
    
    // 从线程映射中移除
    {
        std::lock_guard<std::mutex> lock(threadsMutex_);
        auto it = clientThreads_.find(clientFd);
        if (it != clientThreads_.end()) {
            if (it->second.joinable()) {
                it->second.join();
            }
            clientThreads_.erase(it);
        }
    }
}

void WebSocketServer::run() {
    while (running_) {
        sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);
        int clientFd = ::accept(listenFd_, reinterpret_cast<sockaddr*>(&clientAddr), &clientLen);
        
        if (clientFd < 0) {
            if (running_) {
                std::perror("[WebSocketServer] accept 失败");
            }
            continue;
        }
        
        std::cout << "[WebSocketServer] 新客户端连接: " 
                  << inet_ntoa(clientAddr.sin_addr) << ":" 
                  << ntohs(clientAddr.sin_port) 
                  << " (fd=" << clientFd << ")" << std::endl;
        
        // 处理 WebSocket 握手（在主线程中）
        if (!handleHandshake(clientFd)) {
            std::cout << "[WebSocketServer] 握手失败，关闭连接" << std::endl;
            ::close(clientFd);
            continue;
        }
        
        // 调用连接回调（在主线程中）
        if (onConnect) {
            onConnect(clientFd);
        }
        
        // 创建新线程处理该客户端
        {
            std::lock_guard<std::mutex> lock(threadsMutex_);
            clientThreads_[clientFd] = std::thread(&WebSocketServer::handleClient, this, clientFd);
            clientThreads_[clientFd].detach();  // 分离线程，让它在后台运行
        }
        
        std::cout << "[WebSocketServer] 已为客户端创建处理线程 (fd=" << clientFd << ")" << std::endl;
    }
}

std::string WebSocketServer::sha1(const std::string& input) {
    // 已在 generateHandshakeResponse 中直接使用 OpenSSL SHA1
    return "";
}

std::string WebSocketServer::base64Encode(const std::string& input) {
    // 已在 generateHandshakeResponse 中直接使用 base64EncodeSimple
    return "";
}
