//
// NetGameController.h
// 游戏网络控制器
//
// 职责：
// 1. 解析来自 NetClient 的 JSON 文本，转换为结构化数据
// 2. 根据 type 字段调用 GameLayer 对应的处理函数
// 3. 接收 GameLayer 的用户操作请求，组装 JSON 并通过 NetClient 发送给服务器
//

#ifndef NET_GAME_CONTROLLER_H
#define NET_GAME_CONTROLLER_H

#include "cocos2d.h"
#include <string>
#include <vector>
#include <memory>

// 前向声明，避免循环依赖
class GameLayer;

// 简单的 JSON 解析辅助（如果项目已有 rapidjson 等库，可替换）
// 这里先用字符串解析的简单方式，实际项目中建议使用 rapidjson 或 nlohmann/json
namespace JsonHelper {
    // 简单解析工具函数（实际项目中应使用专业 JSON 库）
    // 这里仅作示例，建议替换为 rapidjson
    std::string getString(const std::string& json, const std::string& key);
    int getInt(const std::string& json, const std::string& key);
    std::vector<int> getIntArray(const std::string& json, const std::string& key);
    std::vector<std::string> getStringArray(const std::string& json, const std::string& key);
}

// 房间信息结构
struct RoomInfo {
    std::string roomId;
    struct PlayerInfo {
        int seat;
        std::string playerId;
        std::string nickname;
    };
    std::vector<PlayerInfo> players;
};

// 结算结果结构
struct RoundResult {
    int winnerSeat;
    std::vector<int> scores;
    struct Detail {
        int winnerFan;
        std::vector<std::string> fanTypes;
    } detail;
};

class NetGameController {
public:
    explicit NetGameController(GameLayer* gameLayer);
    ~NetGameController();
    
    // 处理来自 NetClient 的原始 JSON 消息
    void onRawMessage(const std::string& jsonText);
    
    // ========== UI -> 服务器：发送消息接口 ==========
    
    // 加入房间
    void sendJoinRoom(const std::string& roomId,
                      const std::string& playerId,
                      const std::string& nickname);
    
    // 出牌
    void sendPlayCard(int card);
    
    // 选择动作（碰/杠/胡/过）
    void sendChooseAction(const std::string& action, int card);
    
private:
    GameLayer* gameLayer_;
    
    // ========== 消息处理函数（根据 protocol.md 中的 type 字段分发）==========
    
    void handleRoomInfo(const std::string& jsonText);
    void handleDealCards(const std::string& jsonText);
    void handleYourTurn(const std::string& jsonText);
    void handlePlayerPlayCard(const std::string& jsonText);
    void handleAskAction(const std::string& jsonText);
    void handleRoundResult(const std::string& jsonText);
    void handleError(const std::string& jsonText);
    
    // JSON 组装辅助函数
    std::string buildJson(const std::string& type, const std::vector<std::pair<std::string, std::string>>& fields);
};

#endif // NET_GAME_CONTROLLER_H
