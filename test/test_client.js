#!/usr/bin/env node

/**
 * 麻将服务器测试客户端
 * 
 * 使用方法：
 *   node test_client.js [playerId] [nickname] [roomId]
 * 
 * 示例：
 *   node test_client.js player_001 玩家1 test_room
 */

const WebSocket = require('ws');

// 默认参数
const DEFAULT_PLAYER_ID = process.argv[2] || 'player_' + Math.floor(Math.random() * 1000);
const DEFAULT_NICKNAME = process.argv[3] || '测试玩家' + Math.floor(Math.random() * 100);
const DEFAULT_ROOM_ID = process.argv[4] || 'test_room_001';
const SERVER_URL = process.env.SERVER_URL || 'ws://127.0.0.1:5555';

console.log('========================================');
console.log('麻将服务器测试客户端');
console.log('========================================');
console.log(`服务器: ${SERVER_URL}`);
console.log(`玩家ID: ${DEFAULT_PLAYER_ID}`);
console.log(`昵称: ${DEFAULT_NICKNAME}`);
console.log(`房间ID: ${DEFAULT_ROOM_ID}`);
console.log('========================================\n');

const ws = new WebSocket(SERVER_URL);

// 连接状态
let connected = false;
let inRoom = false;
let mySeat = -1;
let handCards = [];

// 连接成功
ws.on('open', () => {
    console.log('✅ 连接成功！\n');
    connected = true;
    
    // 发送加入房间消息
    const joinMsg = {
        type: 'join_room',
        roomId: DEFAULT_ROOM_ID,
        playerId: DEFAULT_PLAYER_ID,
        nickname: DEFAULT_NICKNAME
    };
    
    console.log('📤 发送加入房间消息:', JSON.stringify(joinMsg, null, 2));
    ws.send(JSON.stringify(joinMsg));
});

// 接收消息
ws.on('message', (data) => {
    try {
        const msg = JSON.parse(data.toString());
        console.log('\n📨 收到消息:', JSON.stringify(msg, null, 2));
        
        handleMessage(msg);
    } catch (e) {
        console.error('❌ 解析消息失败:', e.message);
        console.error('原始数据:', data.toString());
    }
});

// 处理消息
function handleMessage(msg) {
    switch (msg.type) {
        case 'room_info':
            handleRoomInfo(msg);
            break;
            
        case 'deal_cards':
            handleDealCards(msg);
            break;
            
        case 'your_turn':
            handleYourTurn(msg);
            break;
            
        case 'player_play_card':
            handlePlayerPlayCard(msg);
            break;
            
        case 'ask_action':
            handleAskAction(msg);
            break;
            
        case 'round_result':
            handleRoundResult(msg);
            break;
            
        case 'error':
            handleError(msg);
            break;
            
        default:
            console.log('⚠️  未知消息类型:', msg.type);
    }
}

function handleRoomInfo(msg) {
    console.log('\n✅ 加入房间成功！');
    console.log(`房间ID: ${msg.roomId}`);
    console.log(`玩家列表:`);
    
    if (msg.players && Array.isArray(msg.players)) {
        msg.players.forEach(player => {
            console.log(`  座位 ${player.seat}: ${player.nickname} (${player.playerId})`);
            if (player.playerId === DEFAULT_PLAYER_ID) {
                mySeat = player.seat;
            }
        });
    } else {
        console.log('  (玩家列表格式不正确)');
    }
    
    inRoom = true;
}

function handleDealCards(msg) {
    console.log('\n🎴 收到发牌！');
    console.log(`座位: ${msg.seat}, 手牌数量: ${msg.handCards ? msg.handCards.length : 0}`);
    
    if (msg.seat === mySeat) {
        handCards = msg.handCards || [];
        console.log(`手牌: [${handCards.join(', ')}]`);
    } else {
        console.log(`(其他玩家的手牌)`);
    }
}

function handleYourTurn(msg) {
    console.log('\n🎮 轮到你出牌了！');
    console.log(`允许的操作: ${msg.allowedActions ? msg.allowedActions.join(', ') : 'PLAY_CARD'}`);
    
    // 自动出第一张牌（测试用）
    if (handCards.length > 0) {
        const card = handCards[0];
        console.log(`\n📤 自动出牌: ${card}`);
        
        const playMsg = {
            type: 'play_card',
            card: card
        };
        
        ws.send(JSON.stringify(playMsg));
        handCards.shift(); // 从手牌中移除
    } else {
        console.log('⚠️  手牌为空，无法出牌');
    }
}

function handlePlayerPlayCard(msg) {
    console.log(`\n🃏 玩家出牌: 座位 ${msg.seat}, 牌 ${msg.card}`);
}

function handleAskAction(msg) {
    console.log(`\n❓ 询问动作: 牌 ${msg.card}`);
    console.log(`可选动作: ${msg.actions ? msg.actions.join(', ') : 'GUO'}`);
    
    // 自动选择"过"（测试用）
    console.log(`\n📤 自动选择: GUO`);
    
    const actionMsg = {
        type: 'choose_action',
        action: 'GUO',
        card: msg.card
    };
    
    ws.send(JSON.stringify(actionMsg));
}

function handleRoundResult(msg) {
    console.log('\n🏆 游戏结算！');
    console.log(`赢家座位: ${msg.winnerSeat}`);
    console.log(`得分:`);
    
    if (msg.scores && Array.isArray(msg.scores)) {
        msg.scores.forEach((score, index) => {
            console.log(`  座位 ${index}: ${score > 0 ? '+' : ''}${score}`);
        });
    }
    
    if (msg.detail) {
        console.log(`详情:`, JSON.stringify(msg.detail, null, 2));
    }
}

function handleError(msg) {
    console.error(`\n❌ 服务器错误: ${msg.code}`);
    console.error(`消息: ${msg.message}`);
}

// 错误处理
ws.on('error', (error) => {
    console.error('❌ WebSocket 错误:', error.message);
});

// 连接关闭
ws.on('close', () => {
    console.log('\n🔌 连接已关闭');
    connected = false;
    inRoom = false;
});

// 优雅退出
process.on('SIGINT', () => {
    console.log('\n\n正在关闭连接...');
    if (ws.readyState === WebSocket.OPEN) {
        ws.close();
    }
    process.exit(0);
});

// 保持运行
console.log('等待服务器消息... (按 Ctrl+C 退出)\n');
