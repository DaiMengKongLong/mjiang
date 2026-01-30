//
// TestClientExample.cpp
// 测试客户端示例实现
//

#include "TestClientExample.h"
#include "ui/CocosGUI.h"
#include <iostream>

USING_NS_CC;
using namespace ui;

// ========== TestGameLayer 实现 ==========

void TestGameLayer::onRoomInfo(const RoomInfo& info) {
    CCLOG("========== [TestGameLayer] 收到房间信息 ==========");
    CCLOG("房间ID: %s", info.roomId.c_str());
    CCLOG("玩家数量: %zu", info.players.size());
    for (const auto& player : info.players) {
        CCLOG("  座位 %d: %s (ID: %s)", 
              player.seat, player.nickname.c_str(), player.playerId.c_str());
    }
    CCLOG("================================================");
}

void TestGameLayer::onDealCards(int seat, const std::vector<int>& cards) {
    CCLOG("========== [TestGameLayer] 收到发牌 ==========");
    CCLOG("座位: %d, 手牌数量: %zu", seat, cards.size());
    CCLOG("手牌: ");
    for (size_t i = 0; i < cards.size(); ++i) {
        CCLOG("  [%zu] = %d", i, cards[i]);
    }
    CCLOG("==============================================");
}

void TestGameLayer::onYourTurn() {
    CCLOG("========== [TestGameLayer] 轮到你出牌 ==========");
    CCLOG("================================================");
}

void TestGameLayer::onPlayerPlayCard(int seat, int card) {
    CCLOG("========== [TestGameLayer] 玩家出牌 ==========");
    CCLOG("座位: %d, 牌: %d", seat, card);
    CCLOG("==============================================");
}

void TestGameLayer::onAskAction(int card, const std::vector<std::string>& actions) {
    CCLOG("========== [TestGameLayer] 询问动作 ==========");
    CCLOG("牌: %d, 可选动作数量: %zu", card, actions.size());
    for (const auto& action : actions) {
        CCLOG("  动作: %s", action.c_str());
    }
    CCLOG("==============================================");
}

void TestGameLayer::onRoundResult(const RoundResult& result) {
    CCLOG("========== [TestGameLayer] 结算 ==========");
    CCLOG("赢家座位: %d", result.winnerSeat);
    CCLOG("得分: ");
    for (size_t i = 0; i < result.scores.size(); ++i) {
        CCLOG("  座位 %zu: %d", i, result.scores[i]);
    }
    CCLOG("==========================================");
}

// ========== TestClientScene 实现 ==========

bool TestClientScene::init() {
    if (!Scene::init()) {
        return false;
    }
    
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    
    // 创建背景
    auto bg = LayerColor::create(Color4B(50, 50, 50, 255));
    this->addChild(bg);
    
    // 标题
    auto title = Label::createWithTTF("麻将联机测试客户端", "fonts/Marker Felt.ttf", 36);
    title->setPosition(Vec2(origin.x + visibleSize.width / 2, 
                           origin.y + visibleSize.height - 50));
    this->addChild(title);
    
    // 连接按钮
    auto connectBtn = Button::create();
    connectBtn->setTitleText("连接到服务器 (ws://127.0.0.1:5555)");
    connectBtn->setTitleFontSize(24);
    connectBtn->setPosition(Vec2(origin.x + visibleSize.width / 2, 
                                 origin.y + visibleSize.height - 150));
    connectBtn->addClickEventListener(CC_CALLBACK_1(TestClientScene::onConnectButtonClicked, this));
    this->addChild(connectBtn);
    
    // 发送测试消息按钮
    auto sendBtn = Button::create();
    sendBtn->setTitleText("发送测试消息 (join_room)");
    sendBtn->setTitleFontSize(24);
    sendBtn->setPosition(Vec2(origin.x + visibleSize.width / 2, 
                              origin.y + visibleSize.height - 220));
    sendBtn->addClickEventListener(CC_CALLBACK_1(TestClientScene::onSendTestMessageClicked, this));
    this->addChild(sendBtn);
    
    // 状态标签
    auto statusLabel = Label::createWithTTF("未连接", "fonts/Marker Felt.ttf", 20);
    statusLabel->setPosition(Vec2(origin.x + visibleSize.width / 2, 
                                  origin.y + visibleSize.height - 300));
    statusLabel->setName("statusLabel");
    this->addChild(statusLabel);
    
    // 日志显示区域（简化版，实际可以用 ScrollView）
    auto logLabel = Label::createWithTTF("等待连接...\n", "fonts/Marker Felt.ttf", 16);
    logLabel->setPosition(Vec2(origin.x + visibleSize.width / 2, 
                               origin.y + visibleSize.height / 2));
    logLabel->setName("logLabel");
    logLabel->setDimensions(visibleSize.width - 40, visibleSize.height - 400);
    logLabel->setAlignment(TextHAlignment::LEFT, TextVAlignment::TOP);
    this->addChild(logLabel);
    
    // 初始化网络
    setupNetwork();
    
    return true;
}

void TestClientScene::setupNetwork() {
    // 创建 NetGameController（注意：这里需要将 TestGameLayer 适配为 GameLayer 接口）
    // 由于 TestGameLayer 不是 GameLayer 的子类，我们需要一个适配器
    // 为了简化，这里先创建一个临时的 GameLayer 指针包装
    
    // 注意：实际项目中，这里应该传入真正的 GameLayer 实例
    // 这里为了测试，我们创建一个包装类或者修改 NetGameController 使其接受接口
    
    // 简化方案：直接使用 NetGameController，但需要修改其接口以接受回调函数
    // 或者创建一个适配器类
    
    // 这里先注释掉，因为需要修改 NetGameController 的设计
    // netController_ = std::make_unique<NetGameController>(&testLayer_);
    
    auto netClient = NetClient::getInstance();
    
    // 设置消息回调
    netClient->onMessage = [this](const std::string& jsonText) {
        Director::getInstance()->getScheduler()->performFunctionInCocosThread([this, jsonText]() {
            // 更新日志显示
            auto logLabel = dynamic_cast<Label*>(this->getChildByName("logLabel"));
            if (logLabel) {
                std::string currentText = logLabel->getString();
                currentText += "\n[收到消息] " + jsonText;
                logLabel->setString(currentText);
            }
            
            CCLOG("[TestClientScene] 收到消息: %s", jsonText.c_str());
            
            // 这里可以手动解析 JSON 并调用 testLayer_ 的方法
            // 或者创建一个适配器将消息转发给 testLayer_
        });
    };
    
    netClient->onConnected = [this]() {
        auto statusLabel = dynamic_cast<Label*>(this->getChildByName("statusLabel"));
        if (statusLabel) {
            statusLabel->setString("已连接");
            statusLabel->setColor(Color3B::GREEN);
        }
        
        auto logLabel = dynamic_cast<Label*>(this->getChildByName("logLabel"));
        if (logLabel) {
            std::string currentText = logLabel->getString();
            currentText += "\n[系统] 已连接到服务器";
            logLabel->setString(currentText);
        }
        
        CCLOG("[TestClientScene] 已连接到服务器");
    };
    
    netClient->onDisconnected = [this]() {
        auto statusLabel = dynamic_cast<Label*>(this->getChildByName("statusLabel"));
        if (statusLabel) {
            statusLabel->setString("已断开");
            statusLabel->setColor(Color3B::RED);
        }
        
        auto logLabel = dynamic_cast<Label*>(this->getChildByName("logLabel"));
        if (logLabel) {
            std::string currentText = logLabel->getString();
            currentText += "\n[系统] 与服务器断开连接";
            logLabel->setString(currentText);
        }
        
        CCLOG("[TestClientScene] 与服务器断开连接");
    };
    
    netClient->onError = [this](const std::string& errorMsg) {
        auto logLabel = dynamic_cast<Label*>(this->getChildByName("logLabel"));
        if (logLabel) {
            std::string currentText = logLabel->getString();
            currentText += "\n[错误] " + errorMsg;
            logLabel->setString(currentText);
        }
        
        CCLOGERROR("[TestClientScene] 网络错误: %s", errorMsg.c_str());
    };
}

void TestClientScene::onConnectButtonClicked(Ref* sender) {
    auto netClient = NetClient::getInstance();
    
    if (netClient->isConnected()) {
        netClient->close();
    } else {
        // 注意：当前服务器是 TCP，但 NetClient 使用 WebSocket
        // 如果服务器是 TCP，需要修改 NetClient 或使用 TCP 客户端
        // 这里先尝试 WebSocket 连接（如果服务器支持 WebSocket）
        // 如果服务器是纯 TCP，需要修改连接方式
        
        // 临时方案：如果服务器是 TCP，可以创建一个 TCP 版本的客户端
        // 或者修改服务器支持 WebSocket
        
        netClient->connect("ws://127.0.0.1:5555");
    }
}

void TestClientScene::onSendTestMessageClicked(Ref* sender) {
    auto netClient = NetClient::getInstance();
    
    if (!netClient->isConnected()) {
        CCLOGWARN("[TestClientScene] 未连接，无法发送消息");
        return;
    }
    
    // 发送 join_room 测试消息
    std::string json = "{\"type\":\"join_room\",\"roomId\":\"test_room_001\",\"playerId\":\"test_player_001\",\"nickname\":\"测试玩家\"}";
    netClient->sendJson(json);
    
    auto logLabel = dynamic_cast<Label*>(this->getChildByName("logLabel"));
    if (logLabel) {
        std::string currentText = logLabel->getString();
        currentText += "\n[发送消息] " + json;
        logLabel->setString(currentText);
    }
    
    CCLOG("[TestClientScene] 发送测试消息: %s", json.c_str());
}
