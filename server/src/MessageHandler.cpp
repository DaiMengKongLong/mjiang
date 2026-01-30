//
// MessageHandler.cpp
// 消息处理器实现
//

#include "MessageHandler.h"
#include "WebSocketServer.h"
#include "Room.h"
#include "NetPlayer.h"
#include "JsonHelper.h"
#include "game/GameLogic.h"  // 用于 WIK_* 常量
#include <iostream>
#include <sstream>
#include <algorithm>
#include <map>

// ========== MessageHandler 实现 ==========

MessageHandler::MessageHandler(WebSocketServer* server)
    : server_(server) {
}

void MessageHandler::setRoomManager(std::function<std::shared_ptr<Room>(const std::string& roomId)> getOrCreateRoom) {
    getOrCreateRoom_ = getOrCreateRoom;
}

void MessageHandler::handleMessage(int clientFd, const std::string& jsonText) {
    // 解析 type 字段
    std::string type = JsonHelper::getString(jsonText, "type");
    
    std::cout << "[MessageHandler] 收到消息类型: " << type << " (fd=" << clientFd << ")" << std::endl;
    
    if (type == "join_room") {
        handleJoinRoom(clientFd, jsonText);
    } else if (type == "play_card") {
        handlePlayCard(clientFd, jsonText);
    } else if (type == "choose_action") {
        handleChooseAction(clientFd, jsonText);
    } else {
        std::cout << "[MessageHandler] 未知消息类型: " << type << std::endl;
        sendError(clientFd, "UNKNOWN_TYPE", "未知的消息类型: " + type);
    }
}

void MessageHandler::handleJoinRoom(int clientFd, const std::string& jsonText) {
    std::string roomId = JsonHelper::getString(jsonText, "roomId");
    std::string playerId = JsonHelper::getString(jsonText, "playerId");
    std::string nickname = JsonHelper::getString(jsonText, "nickname");
    
    if (roomId.empty()) {
        // 如果 roomId 为空，服务器分配一个
        roomId = "room_" + std::to_string(clientFd);
    }
    
    if (playerId.empty() || nickname.empty()) {
        sendError(clientFd, "INVALID_PARAMS", "playerId 和 nickname 不能为空");
        return;
    }
    
    std::cout << "[MessageHandler] 玩家加入房间: roomId=" << roomId 
              << ", playerId=" << playerId << ", nickname=" << nickname << std::endl;
    
    // 获取或创建房间
    if (!getOrCreateRoom_) {
        sendError(clientFd, "SERVER_ERROR", "房间管理器未设置");
        return;
    }
    
    auto room = getOrCreateRoom_(roomId);
    
    // 创建 NetPlayer（需要 clientFd 和 server 指针）
    auto player = std::make_shared<NetPlayer>(playerId, clientFd, server_);
    player->setNickname(nickname);
    
    // 添加到房间（Room 会自动分配座位）
    if (!room->addPlayer(player)) {
        sendError(clientFd, "ROOM_FULL", "房间已满或无法加入");
        return;
    }
    
    int seat = player->getSeat();
    
    // 保存客户端信息
    ClientInfo info;
    info.room = room;
    info.playerId = playerId;
    info.nickname = nickname;
    info.seat = seat;
    clients_[clientFd] = info;
    
    // 向所有玩家发送更新后的房间信息
    sendRoomInfoToAll(room);
    
    // 如果房间有 4 个玩家，自动开始游戏（简化版）
    if (room->getPlayerCount() >= 4) {
        std::cout << "[MessageHandler] 房间已满，开始游戏" << std::endl;
        room->startGameMock();
        
        // 向所有玩家发送发牌消息（假数据）
        // 这里简化处理，只向当前客户端发送
        std::string dealCards = R"({"type":"deal_cards","seat":)" + std::to_string(seat) 
                              + R"(,"handCards":[1,2,3,4,5,6,7,8,9,10,11,12,13]})";
        server_->sendText(clientFd, dealCards);
        
        // 发送轮到你出牌的消息（简化版：总是让 seat 0 先出）
        if (seat == 0) {
            std::string yourTurn = R"({"type":"your_turn","allowedActions":["PLAY_CARD"]})";
            server_->sendText(clientFd, yourTurn);
        }
    }
}

void MessageHandler::handlePlayCard(int clientFd, const std::string& jsonText) {
    // 获取客户端信息
    std::lock_guard<std::mutex> lock(clientsMutex_);
    auto it = clients_.find(clientFd);
    if (it == clients_.end()) {
        sendError(clientFd, "NOT_IN_ROOM", "玩家未加入房间");
        return;
    }
    
    auto room = it->second.room;
    if (!room) {
        sendError(clientFd, "ROOM_NOT_FOUND", "房间不存在");
        return;
    }
    
    // 检查房间状态
    if (room->getState() != RoomState::PLAYING) {
        sendError(clientFd, "ROOM_NOT_PLAYING", "房间不在游戏中");
        return;
    }
    
    // 获取出牌数据
    int card = JsonHelper::getInt(jsonText, "card");
    if (card < 0 || card > 255) {
        sendError(clientFd, "INVALID_CARD", "无效的牌");
        return;
    }
    
#ifdef USE_GAME_ENGINE
    // 调用 GameEngine 的出牌方法
    auto gameEngine = room->getGameEngine();
    if (!gameEngine) {
        sendError(clientFd, "GAME_ENGINE_ERROR", "游戏引擎未初始化");
        return;
    }
    
    CMD_C_OutCard outCard;
    outCard.cbCardData = static_cast<uint8_t>(card);
    
    if (gameEngine->onUserOutCard(outCard)) {
        std::cout << "[MessageHandler] 玩家出牌成功: playerId=" << it->second.playerId 
                  << ", card=" << card << std::endl;
    } else {
        sendError(clientFd, "PLAY_CARD_FAILED", "出牌失败");
    }
#else
    // 未启用 GameEngine，使用简化版
    std::cout << "[MessageHandler] 玩家出牌: playerId=" << it->second.playerId 
              << ", seat=" << it->second.seat << ", card=" << card << std::endl;
    std::string response = R"({"type":"player_play_card","seat":)" + std::to_string(it->second.seat)
                          + R"(,"card":)" + std::to_string(card) + "}";
    server_->sendText(clientFd, response);
#endif
}

void MessageHandler::handleChooseAction(int clientFd, const std::string& jsonText) {
    // 获取客户端信息
    std::lock_guard<std::mutex> lock(clientsMutex_);
    auto it = clients_.find(clientFd);
    if (it == clients_.end()) {
        sendError(clientFd, "NOT_IN_ROOM", "玩家未加入房间");
        return;
    }
    
    auto room = it->second.room;
    if (!room) {
        sendError(clientFd, "ROOM_NOT_FOUND", "房间不存在");
        return;
    }
    
    // 检查房间状态
    if (room->getState() != RoomState::PLAYING) {
        sendError(clientFd, "ROOM_NOT_PLAYING", "房间不在游戏中");
        return;
    }
    
    // 获取动作和牌
    std::string action = JsonHelper::getString(jsonText, "action");
    int card = JsonHelper::getInt(jsonText, "card");
    
    // 转换动作字符串到操作码
    uint8_t operateCode = 0;
    if (action == "PENG") {
        operateCode = WIK_P;
    } else if (action == "GANG") {
        operateCode = WIK_G;
    } else if (action == "HU") {
        operateCode = WIK_H;
    } else if (action == "GUO") {
        operateCode = WIK_NULL;
    } else {
        sendError(clientFd, "INVALID_ACTION", "无效的动作");
        return;
    }
    
#ifdef USE_GAME_ENGINE
    // 调用 GameEngine 的操作方法
    auto gameEngine = room->getGameEngine();
    if (!gameEngine) {
        sendError(clientFd, "GAME_ENGINE_ERROR", "游戏引擎未初始化");
        return;
    }
    
    CMD_C_OperateCard operateCard;
    operateCard.cbOperateUser = static_cast<uint8_t>(it->second.seat);
    operateCard.cbOperateCode = operateCode;
    operateCard.cbOperateCard = static_cast<uint8_t>(card);
    
    if (gameEngine->onUserOperateCard(operateCard)) {
        std::cout << "[MessageHandler] 玩家选择动作成功: playerId=" << it->second.playerId 
                  << ", action=" << action << ", card=" << card << std::endl;
    } else {
        sendError(clientFd, "ACTION_FAILED", "动作执行失败");
    }
#else
    // 未启用 GameEngine，使用简化版
    std::cout << "[MessageHandler] 玩家选择动作: playerId=" << it->second.playerId 
              << ", action=" << action << ", card=" << card << std::endl;
    std::string response = R"({"type":"action_confirmed","action":")" + action 
                          + R"(","card":)" + std::to_string(card) + "}";
    server_->sendText(clientFd, response);
#endif
}

void MessageHandler::sendRoomInfo(int clientFd, std::shared_ptr<Room> room) {
    std::ostringstream oss;
    
    // 获取房间状态字符串
    std::string stateStr;
    switch (room->getState()) {
        case RoomState::WAITING:
            stateStr = "WAITING";
            break;
        case RoomState::PLAYING:
            stateStr = "PLAYING";
            break;
        case RoomState::FINISHED:
            stateStr = "FINISHED";
            break;
    }
    
    oss << R"({"type":"room_info","roomId":")" << room->getId() 
        << R"(","state":")" << stateStr << R"(","players":[)";
    
    auto players = room->getPlayers();  // 获取玩家列表的副本（线程安全）
    bool first = true;
    for (const auto& player : players) {
        if (!first) oss << ",";
        oss << R"({"seat":)" << player->getSeat()
            << R"(,"playerId":")" << player->getPlayerId()
            << R"(","nickname":")" << player->getNickname() << "\"}";
        first = false;
    }
    
    oss << "]}";
    server_->sendText(clientFd, oss.str());
}

void MessageHandler::sendRoomInfoToAll(std::shared_ptr<Room> room) {
    // 获取房间状态字符串
    std::string stateStr;
    switch (room->getState()) {
        case RoomState::WAITING:
            stateStr = "WAITING";
            break;
        case RoomState::PLAYING:
            stateStr = "PLAYING";
            break;
        case RoomState::FINISHED:
            stateStr = "FINISHED";
            break;
    }
    
    // 构建房间信息 JSON
    std::ostringstream oss;
    oss << R"({"type":"room_info","roomId":")" << room->getId() 
        << R"(","state":")" << stateStr << R"(","players":[)";
    
    const auto& players = room->getPlayers();
    bool first = true;
    for (const auto& player : players) {
        if (!first) oss << ",";
        oss << R"({"seat":)" << player->getSeat()
            << R"(,"playerId":")" << player->getPlayerId()
            << R"(","nickname":")" << player->getNickname() << "\"}";
        first = false;
    }
    
    oss << "]}";
    std::string roomInfoJson = oss.str();
    
    // 向房间内所有玩家发送（线程安全）
    {
        std::lock_guard<std::mutex> lock(clientsMutex_);
        for (auto& pair : clients_) {
            if (pair.second.room == room) {
                server_->sendText(pair.first, roomInfoJson);
            }
        }
    }
    
    std::cout << "[MessageHandler] 向房间 " << room->getId() 
              << " 的所有玩家发送房间信息" << std::endl;
}

void MessageHandler::cleanupClient(int clientFd) {
    std::shared_ptr<Room> room;
    std::string playerId;
    
    // 从客户端映射中获取信息并移除（线程安全）
    {
        std::lock_guard<std::mutex> lock(clientsMutex_);
        auto it = clients_.find(clientFd);
        if (it == clients_.end()) {
            return;
        }
        
        room = it->second.room;
        playerId = it->second.playerId;
        int seat = it->second.seat;
        
        std::cout << "[MessageHandler] 清理客户端: fd=" << clientFd 
                  << ", playerId=" << playerId << ", seat=" << seat << std::endl;
        
        // 从客户端映射中移除
        clients_.erase(it);
    }
    
    // 从房间中移除玩家
    if (room) {
        room->removePlayer(playerId);
        
        // 如果房间还有玩家，通知他们更新
        if (room->getPlayerCount() > 0) {
            sendRoomInfoToAll(room);
        }
    }
}

void MessageHandler::sendError(int clientFd, const std::string& code, const std::string& message) {
    std::ostringstream oss;
    oss << R"({"type":"error","code":")" << code 
        << R"(","message":")" << message << "\"}";
    
    server_->sendText(clientFd, oss.str());
    std::cout << "[MessageHandler] 发送错误: code=" << code << ", message=" << message << std::endl;
}
