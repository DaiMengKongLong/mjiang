// 测试游戏开始功能 - 4个玩家加入同一房间
const WebSocket = require('ws');

const playerId = process.argv[2] || 'player_001';
const nickname = process.argv[3] || '测试玩家';
const roomId = process.argv[4] || 'test_room_4players';

console.log(`🚀 玩家 ${nickname} (${playerId}) 连接到服务器 ws://127.0.0.1:5555`);
console.log(`📋 房间ID: ${roomId}`);
console.log('');

const ws = new WebSocket('ws://127.0.0.1:5555');

let messageCount = 0;

ws.on('open', () => {
    console.log('✅ 连接成功！');
    console.log(`📤 发送加入房间请求...`);
    
    ws.send(JSON.stringify({
        type: 'join_room',
        roomId: roomId,
        playerId: playerId,
        nickname: nickname
    }));
});

ws.on('message', (data) => {
    try {
        const msg = JSON.parse(data.toString());
        messageCount++;
        
        console.log(`📨 [消息 ${messageCount}] 收到:`, JSON.stringify(msg, null, 2));
        
        // 游戏开始消息
        if (msg.type === 'game_start') {
            console.log('\n🎮 ========== 游戏开始！==========');
            console.log(`   骰子点数: ${msg.diceCount}`);
            console.log(`   庄家: 座位 ${msg.bankerUser}`);
            console.log(`   当前玩家: 座位 ${msg.currentUser}`);
            console.log(`   剩余牌数: ${msg.leftCardCount}`);
            if (msg.cards && msg.cards.length > 0) {
                console.log(`   手牌数量: ${msg.cards.length}`);
                console.log(`   手牌: ${msg.cards.join(', ')}`);
            }
            console.log('================================\n');
        }
        
        // 发牌消息
        if (msg.type === 'deal_cards') {
            console.log(`\n🃏 发牌: 牌=${msg.card}, 当前玩家=座位${msg.currentUser}`);
            if (msg.gangCards && msg.gangCards.length > 0) {
                console.log(`   可杠牌: ${msg.gangCards.join(', ')}`);
            }
        }
        
        // 房间信息
        if (msg.type === 'room_info') {
            console.log(`\n📋 房间信息: 状态=${msg.state}, 玩家数=${msg.players ? msg.players.length : 0}`);
            if (msg.players) {
                msg.players.forEach((p, i) => {
                    console.log(`   玩家${i + 1}: ${p.nickname} (${p.playerId}) - 座位${p.seat}`);
                });
            }
            
            // 如果房间状态变为 PLAYING，说明游戏已开始
            if (msg.state === 'PLAYING') {
                console.log('\n🎮 房间状态已变为 PLAYING（游戏中）');
            }
        }
        
        // 出牌消息
        if (msg.type === 'player_play_card') {
            console.log(`\n🎴 玩家出牌: 座位${msg.seat} 打出 牌${msg.card}`);
        }
        
        // 操作通知
        if (msg.type === 'ask_action') {
            console.log(`\n❓ 询问动作: 动作掩码=${msg.actionMask}, 牌=${msg.actionCard}`);
        }
        
        // 游戏结束
        if (msg.type === 'round_result') {
            console.log(`\n🏁 ========== 游戏结束 ==========`);
            console.log(`   胡牌玩家: 座位${msg.huUser}`);
            console.log(`   供牌玩家: 座位${msg.provideUser}`);
            console.log(`   胡牌: ${msg.huCard}`);
            if (msg.scores) {
                msg.scores.forEach((s, i) => {
                    console.log(`   玩家${i} (座位${s.seat}): 得分=${s.score}`);
                });
            }
            console.log('================================\n');
        }
        
        // 错误消息
        if (msg.type === 'error') {
            console.log(`\n❌ 错误: ${msg.code} - ${msg.message}`);
        }
        
    } catch (e) {
        console.log('📨 收到原始消息:', data.toString());
    }
});

ws.on('error', (error) => {
    console.error('❌ 连接错误:', error.message);
});

ws.on('close', () => {
    console.log('\n🔌 连接已关闭');
    console.log(`📊 总共收到 ${messageCount} 条消息`);
    process.exit(0);
});

// 保持连接，等待游戏开始
console.log('⏳ 等待游戏开始...（按 Ctrl+C 退出）\n');
