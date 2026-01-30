#include "NetPlayer.h"
#include "WebSocketServer.h"
#include "JsonHelper.h"
#include <iostream>
#include <sstream>

NetPlayer::NetPlayer(const std::string& playerId, int clientFd, WebSocketServer* server)
    : IPlayer(false, IPlayer::MALE, this)  // 不是机器人，默认男性
    , playerId_(playerId)
    , clientFd_(clientFd)
    , server_(server)
    , seat_(-1) {
    setGameEngineEventListener(this);
}

void NetPlayer::setIPlayer(IPlayer *pIPlayer) {
    // 实现接口，但不需要特殊处理
}

bool NetPlayer::onUserEnterEvent(IPlayer *pIPlayer) {
    // 玩家进入事件，已经在 join_room 时处理
    return true;
}

bool NetPlayer::onGameStartEvent(CMD_S_GameStart GameStart) {
    // 游戏开始事件
    std::ostringstream oss;
    oss << R"({"type":"game_start","diceCount":)" << GameStart.iDiceCount
        << R"(,"bankerUser":)" << static_cast<int>(GameStart.cbBankerUser)
        << R"(,"currentUser":)" << static_cast<int>(GameStart.cbCurrentUser)
        << R"(,"leftCardCount":)" << static_cast<int>(GameStart.cbLeftCardCount)
        << R"(,"cards":[)";
    
    // 发送手牌（只发送当前玩家的牌）
    // cbCardData 格式：[玩家0的13张, 玩家1的13张, 玩家2的13张, 玩家3的13张]
    bool first = true;
    int startIndex = static_cast<int>(seat_) * MAX_COUNT;
    for (int i = 0; i < MAX_COUNT; i++) {
        uint8_t card = GameStart.cbCardData[startIndex + i];
        // 只发送有效的牌（0-33 范围，0表示空位）
        if (card > 0 && card <= 33) {
            if (!first) oss << ",";
            oss << static_cast<int>(card);
            first = false;
        }
    }
    
    oss << "]}";
    sendJson(oss.str());
    return true;
}

bool NetPlayer::onSendCardEvent(CMD_S_SendCard SendCard) {
    // 发牌事件
    std::ostringstream oss;
    oss << R"({"type":"deal_cards","card":)" << static_cast<int>(SendCard.cbCardData)
        << R"(,"actionMask":)" << static_cast<int>(SendCard.cbActionMask)
        << R"(,"currentUser":)" << static_cast<int>(SendCard.cbCurrentUser)
        << R"(,"isTail":)" << (SendCard.bTail ? "true" : "false");
    
    if (SendCard.cbGangCount > 0) {
        oss << R"(,"gangCount":)" << static_cast<int>(SendCard.cbGangCount)
            << R"(,"gangCards":[)";
        bool first = true;
        for (int i = 0; i < SendCard.cbGangCount; i++) {
            if (!first) oss << ",";
            oss << static_cast<int>(SendCard.cbGangCard[i]);
            first = false;
        }
        oss << "]";
    }
    
    oss << "}";
    sendJson(oss.str());
    return true;
}

bool NetPlayer::onOutCardEvent(CMD_S_OutCard OutCard) {
    // 出牌事件
    std::ostringstream oss;
    oss << R"({"type":"player_play_card","seat":)" << static_cast<int>(OutCard.cbOutCardUser)
        << R"(,"card":)" << static_cast<int>(OutCard.cbOutCardData) << "}";
    sendJson(oss.str());
    return true;
}

bool NetPlayer::onOperateNotifyEvent(CMD_S_OperateNotify OperateNotify) {
    // 操作通知事件（询问是否可以吃碰杠胡）
    if (seat_ == OperateNotify.cbResumeUser) {
        // 只通知当前玩家
        std::ostringstream oss;
        oss << R"({"type":"ask_action","actionMask":)" << static_cast<int>(OperateNotify.cbActionMask)
            << R"(,"actionCard":)" << static_cast<int>(OperateNotify.cbActionCard);
        
        if (OperateNotify.cbGangCount > 0) {
            oss << R"(,"gangCount":)" << static_cast<int>(OperateNotify.cbGangCount)
                << R"(,"gangCards":[)";
            bool first = true;
            for (int i = 0; i < OperateNotify.cbGangCount; i++) {
                if (!first) oss << ",";
                oss << static_cast<int>(OperateNotify.cbGangCard[i]);
                first = false;
            }
            oss << "]";
        }
        
        oss << "}";
        sendJson(oss.str());
    }
    return true;
}

bool NetPlayer::onOperateResultEvent(CMD_S_OperateResult OperateResult) {
    // 操作结果事件
    std::ostringstream oss;
    oss << R"({"type":"action_result","operateUser":)" << static_cast<int>(OperateResult.cbOperateUser)
        << R"(,"provideUser":)" << static_cast<int>(OperateResult.cbProvideUser)
        << R"(,"operateCode":)" << static_cast<int>(OperateResult.cbOperateCode)
        << R"(,"operateCard":)" << static_cast<int>(OperateResult.cbOperateCard) << "}";
    sendJson(oss.str());
    return true;
}

bool NetPlayer::onGameEndEvent(CMD_S_GameEnd GameEnd) {
    // 游戏结束事件
    std::ostringstream oss;
    oss << R"({"type":"round_result","huUser":)" << static_cast<int>(GameEnd.cbHuUser)
        << R"(,"provideUser":)" << static_cast<int>(GameEnd.cbProvideUser)
        << R"(,"huCard":)" << static_cast<int>(GameEnd.cbHuCard)
        << R"(,"scores":[)";
    
    bool first = true;
    for (int i = 0; i < GAME_PLAYER; i++) {
        if (!first) oss << ",";
        oss << R"({"seat":)" << i
            << R"(,"score":)" << GameEnd.lGameScore[i]
            << R"(,"huRight":)" << GameEnd.dwHuRight[i]
            << R"(,"huKind":)" << static_cast<int>(GameEnd.cbHuKind[i])
            << "}";
        first = false;
    }
    
    oss << "]}";
    sendJson(oss.str());
    return true;
}

void NetPlayer::sendJson(const std::string& json) {
    if (server_ && clientFd_ > 0) {
        server_->sendText(clientFd_, json);
        std::cout << "[NetPlayer] 发送消息到 " << playerId_ << ": " << json << std::endl;
    }
}