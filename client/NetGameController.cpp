//
// NetGameController.cpp
// 游戏网络控制器实现
//

#include "NetGameController.h"
#include "NetClient.h"
#include "GameLayer.h"  // 需要根据实际项目路径调整
#include <sstream>
#include <algorithm>

// ========== JsonHelper 简单实现（实际项目中应使用 rapidjson）==========

namespace JsonHelper {
    // 注意：这是非常简单的字符串解析，仅用于演示
    // 实际项目中强烈建议使用 rapidjson 或 nlohmann/json
    
    std::string getString(const std::string& json, const std::string& key) {
        std::string searchKey = "\"" + key + "\"";
        size_t pos = json.find(searchKey);
        if (pos == std::string::npos) return "";
        
        pos = json.find(":", pos);
        if (pos == std::string::npos) return "";
        pos++;
        
        // 跳过空格
        while (pos < json.length() && (json[pos] == ' ' || json[pos] == '\t')) pos++;
        
        if (pos >= json.length()) return "";
        
        // 如果是字符串值
        if (json[pos] == '"') {
            pos++;
            size_t end = json.find('"', pos);
            if (end == std::string::npos) return "";
            return json.substr(pos, end - pos);
        }
        
        return "";
    }
    
    int getInt(const std::string& json, const std::string& key) {
        std::string searchKey = "\"" + key + "\"";
        size_t pos = json.find(searchKey);
        if (pos == std::string::npos) return 0;
        
        pos = json.find(":", pos);
        if (pos == std::string::npos) return 0;
        pos++;
        
        while (pos < json.length() && (json[pos] == ' ' || json[pos] == '\t')) pos++;
        
        int value = 0;
        bool negative = false;
        if (pos < json.length() && json[pos] == '-') {
            negative = true;
            pos++;
        }
        
        while (pos < json.length() && json[pos] >= '0' && json[pos] <= '9') {
            value = value * 10 + (json[pos] - '0');
            pos++;
        }
        
        return negative ? -value : value;
    }
    
    std::vector<int> getIntArray(const std::string& json, const std::string& key) {
        std::vector<int> result;
        std::string searchKey = "\"" + key + "\"";
        size_t pos = json.find(searchKey);
        if (pos == std::string::npos) return result;
        
        pos = json.find("[", pos);
        if (pos == std::string::npos) return result;
        pos++;
        
        size_t end = json.find("]", pos);
        if (end == std::string::npos) return result;
        
        std::string arrayStr = json.substr(pos, end - pos);
        std::istringstream iss(arrayStr);
        std::string token;
        while (std::getline(iss, token, ',')) {
            // 去除空格
            token.erase(0, token.find_first_not_of(" \t"));
            token.erase(token.find_last_not_of(" \t") + 1);
            if (!token.empty()) {
                result.push_back(std::stoi(token));
            }
        }
        
        return result;
    }
    
    std::vector<std::string> getStringArray(const std::string& json, const std::string& key) {
        std::vector<std::string> result;
        std::string searchKey = "\"" + key + "\"";
        size_t pos = json.find(searchKey);
        if (pos == std::string::npos) return result;
        
        pos = json.find("[", pos);
        if (pos == std::string::npos) return result;
        pos++;
        
        size_t end = json.find("]", pos);
        if (end == std::string::npos) return result;
        
        std::string arrayStr = json.substr(pos, end - pos);
        std::istringstream iss(arrayStr);
        std::string token;
        while (std::getline(iss, token, ',')) {
            // 去除空格和引号
            token.erase(0, token.find_first_not_of(" \t\""));
            token.erase(token.find_last_not_of(" \t\"") + 1);
            if (!token.empty()) {
                result.push_back(token);
            }
        }
        
        return result;
    }
}

// ========== NetGameController 实现 ==========

NetGameController::NetGameController(GameLayer* gameLayer)
    : gameLayer_(gameLayer) {
}

NetGameController::~NetGameController() {
}

void NetGameController::onRawMessage(const std::string& jsonText) {
    // 解析 type 字段
    std::string type = JsonHelper::getString(jsonText, "type");
    
    CCLOG("[NetGameController] 收到消息类型: %s", type.c_str());
    
    // 根据 type 分发到对应的处理函数
    if (type == "room_info") {
        handleRoomInfo(jsonText);
    } else if (type == "deal_cards") {
        handleDealCards(jsonText);
    } else if (type == "your_turn") {
        handleYourTurn(jsonText);
    } else if (type == "player_play_card") {
        handlePlayerPlayCard(jsonText);
    } else if (type == "ask_action") {
        handleAskAction(jsonText);
    } else if (type == "round_result") {
        handleRoundResult(jsonText);
    } else if (type == "error") {
        handleError(jsonText);
    } else {
        CCLOGWARN("[NetGameController] 未知消息类型: %s", type.c_str());
    }
}

// ========== 消息处理函数实现 ==========

void NetGameController::handleRoomInfo(const std::string& jsonText) {
    RoomInfo info;
    info.roomId = JsonHelper::getString(jsonText, "roomId");
    
    // 解析 players 数组（简化版，实际应使用 rapidjson）
    // 这里假设 players 数组格式为：[{"seat":0,"playerId":"xxx","nickname":"yyy"}, ...]
    // 实际项目中应使用专业 JSON 库解析嵌套数组
    
    CCLOG("[NetGameController] 房间信息: roomId=%s", info.roomId.c_str());
    
    // 调用 GameLayer 的回调
    if (gameLayer_) {
        gameLayer_->onRoomInfo(info);
    }
}

void NetGameController::handleDealCards(const std::string& jsonText) {
    int seat = JsonHelper::getInt(jsonText, "seat");
    std::vector<int> handCards = JsonHelper::getIntArray(jsonText, "handCards");
    
    CCLOG("[NetGameController] 发牌: seat=%d, cards_count=%zu", seat, handCards.size());
    
    if (gameLayer_) {
        gameLayer_->onDealCards(seat, handCards);
    }
}

void NetGameController::handleYourTurn(const std::string& jsonText) {
    std::vector<std::string> allowedActions = JsonHelper::getStringArray(jsonText, "allowedActions");
    
    CCLOG("[NetGameController] 轮到你出牌");
    
    if (gameLayer_) {
        gameLayer_->onYourTurn();
    }
}

void NetGameController::handlePlayerPlayCard(const std::string& jsonText) {
    int seat = JsonHelper::getInt(jsonText, "seat");
    int card = JsonHelper::getInt(jsonText, "card");
    
    CCLOG("[NetGameController] 玩家出牌: seat=%d, card=%d", seat, card);
    
    if (gameLayer_) {
        gameLayer_->onPlayerPlayCard(seat, card);
    }
}

void NetGameController::handleAskAction(const std::string& jsonText) {
    int card = JsonHelper::getInt(jsonText, "card");
    std::vector<std::string> actions = JsonHelper::getStringArray(jsonText, "actions");
    
    CCLOG("[NetGameController] 询问动作: card=%d, actions_count=%zu", card, actions.size());
    
    if (gameLayer_) {
        gameLayer_->onAskAction(card, actions);
    }
}

void NetGameController::handleRoundResult(const std::string& jsonText) {
    RoundResult result;
    result.winnerSeat = JsonHelper::getInt(jsonText, "winnerSeat");
    result.scores = JsonHelper::getIntArray(jsonText, "scores");
    
    CCLOG("[NetGameController] 结算: winnerSeat=%d", result.winnerSeat);
    
    if (gameLayer_) {
        gameLayer_->onRoundResult(result);
    }
}

void NetGameController::handleError(const std::string& jsonText) {
    std::string code = JsonHelper::getString(jsonText, "code");
    std::string message = JsonHelper::getString(jsonText, "message");
    
    CCLOGERROR("[NetGameController] 服务器错误: code=%s, message=%s", code.c_str(), message.c_str());
    
    // 可以在这里显示错误提示框
    if (gameLayer_) {
        // gameLayer_->showError(code, message);
    }
}

// ========== 发送消息接口实现 ==========

void NetGameController::sendJoinRoom(const std::string& roomId,
                                      const std::string& playerId,
                                      const std::string& nickname) {
    std::ostringstream oss;
    oss << "{\"type\":\"join_room\",\"roomId\":\"" << roomId
        << "\",\"playerId\":\"" << playerId
        << "\",\"nickname\":\"" << nickname << "\"}";
    
    NetClient::getInstance()->sendJson(oss.str());
}

void NetGameController::sendPlayCard(int card) {
    std::ostringstream oss;
    oss << "{\"type\":\"play_card\",\"card\":" << card << "}";
    
    NetClient::getInstance()->sendJson(oss.str());
}

void NetGameController::sendChooseAction(const std::string& action, int card) {
    std::ostringstream oss;
    oss << "{\"type\":\"choose_action\",\"action\":\"" << action
        << "\",\"card\":" << card << "}";
    
    NetClient::getInstance()->sendJson(oss.str());
}
