//
// TestClientExample.h
// 测试客户端示例（用于快速验证网络连接和协议解析）
//
// 说明：
// 这是一个独立的测试类，不依赖原项目的 GameLayer，
// 可以用来验证 NetClient 和 NetGameController 是否正常工作。
//

#ifndef TEST_CLIENT_EXAMPLE_H
#define TEST_CLIENT_EXAMPLE_H

#include "cocos2d.h"
#include "NetClient.h"
#include "NetGameController.h"
#include <string>
#include <vector>

USING_NS_CC;

// 简单的测试 GameLayer 替代类
class TestGameLayer {
public:
    void onRoomInfo(const RoomInfo& info);
    void onDealCards(int seat, const std::vector<int>& cards);
    void onYourTurn();
    void onPlayerPlayCard(int seat, int card);
    void onAskAction(int card, const std::vector<std::string>& actions);
    void onRoundResult(const RoundResult& result);
};

// 测试场景类（可以添加到 Cocos 项目中作为测试场景）
class TestClientScene : public Scene {
public:
    CREATE_FUNC(TestClientScene);
    virtual bool init() override;
    
private:
    std::unique_ptr<NetGameController> netController_;
    TestGameLayer testLayer_;
    
    void setupNetwork();
    void onConnectButtonClicked(Ref* sender);
    void onSendTestMessageClicked(Ref* sender);
};

#endif // TEST_CLIENT_EXAMPLE_H
