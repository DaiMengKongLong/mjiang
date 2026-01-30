//
// NetClient.cpp
// 网络客户端封装类实现
//

#include "NetClient.h"
#include <iostream>

NetClient* NetClient::instance_ = nullptr;

NetClient* NetClient::getInstance() {
    if (!instance_) {
        instance_ = new NetClient();
        instance_->autorelease();
    }
    return instance_;
}

NetClient::NetClient()
    : websocket_(nullptr)
    , connected_(false) {
}

NetClient::~NetClient() {
    close();
}

bool NetClient::connect(const std::string& url) {
    if (connected_ && websocket_) {
        CCLOG("[NetClient] 已连接，先关闭旧连接");
        close();
    }
    
    CCLOG("[NetClient] 尝试连接到: %s", url.c_str());
    
    websocket_ = new WebSocket();
    if (!websocket_->init(*this, url)) {
        CCLOGERROR("[NetClient] WebSocket 初始化失败");
        delete websocket_;
        websocket_ = nullptr;
        return false;
    }
    
    return true;
}

void NetClient::close() {
    if (websocket_) {
        websocket_->close();
        websocket_->release();
        websocket_ = nullptr;
    }
    connected_ = false;
}

void NetClient::sendJson(const std::string& jsonText) {
    if (!connected_ || !websocket_) {
        CCLOGWARN("[NetClient] 未连接，无法发送: %s", jsonText.c_str());
        return;
    }
    
    CCLOG("[NetClient] 发送: %s", jsonText.c_str());
    websocket_->send(jsonText);
}

// ========== WebSocket 事件回调 ==========

void NetClient::onOpen(WebSocket* ws) {
    CCLOG("[NetClient] 连接已建立");
    connected_ = true;
    if (onConnected) {
        onConnected();
    }
}

void NetClient::onMessage(WebSocket* ws, const WebSocket::Data& data) {
    std::string jsonText;
    if (data.isBinary) {
        // 如果是二进制数据，这里需要根据实际协议转换
        // 当前假设服务器发送的是文本 JSON
        CCLOGWARN("[NetClient] 收到二进制数据，暂不支持");
        return;
    } else {
        jsonText = std::string(data.bytes, data.len);
    }
    
    CCLOG("[NetClient] 收到: %s", jsonText.c_str());
    
    // 调用外部注册的回调
    if (onMessage) {
        // 注意：WebSocket 回调可能在非主线程，如果需要在主线程更新 UI，
        // 应该使用 Director::getInstance()->getScheduler()->performFunctionInCocosThread
        // 这里先直接调用，后续根据实际需求调整
        onMessage(jsonText);
    }
}

void NetClient::onClose(WebSocket* ws) {
    CCLOG("[NetClient] 连接已关闭");
    connected_ = false;
    if (onDisconnected) {
        onDisconnected();
    }
}

void NetClient::onError(WebSocket* ws, const WebSocket::ErrorCode& error) {
    std::string errorMsg = "未知错误";
    switch (error) {
        case WebSocket::ErrorCode::TIME_OUT:
            errorMsg = "连接超时";
            break;
        case WebSocket::ErrorCode::CONNECTION_FAILURE:
            errorMsg = "连接失败";
            break;
        case WebSocket::ErrorCode::UNKNOWN:
            errorMsg = "未知错误";
            break;
    }
    
    CCLOGERROR("[NetClient] 错误: %s", errorMsg.c_str());
    connected_ = false;
    
    if (onError) {
        onError(errorMsg);
    }
}
