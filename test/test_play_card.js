// 测试出牌功能
const WebSocket = require('ws');

const playerId = process.argv[2] || 'player_001';
const nickname = process.argv[3] || '测试玩家';
const roomId = process.argv[4] || 'test_room_play';

console.log(`🚀 玩家 ${nickname} (${playerId}) 连接到服务器`);
console.log(`📋 房间ID: ${roomId}`);
console.log('');

const ws = new WebSocket('ws://127.0.0.1:5555');

let gameStarted = false;
let myCards = [];
let currentUser = -1;
let mySeat = -1;

ws.on('open', () => {
    console.log('✅ 连接成功！');
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
        
        // 房间信息
        if (msg.type === 'room_info') {
            console.log(`📋 房间信息: 状态=${msg.state}, 玩家数=${msg.players ? msg.players.length : 0}`);
            if (msg.players) {
                const me = msg.players.find(p => p.playerId === playerId);
                if (me) {
                    mySeat = me.seat;
                    console.log(`   我的座位: ${mySeat}`);
                }
            }
        }
        
        // 游戏开始
        if (msg.type === 'game_start') {
            gameStarted = true;
            myCards = msg.cards || [];
            currentUser = msg.currentUser;
            console.log(`\n🎮 游戏开始！`);
            console.log(`   庄家: 座位 ${msg.bankerUser}`);
            console.log(`   当前玩家: 座位 ${msg.currentUser}`);
            console.log(`   我的手牌 (${myCards.length}张): ${myCards.join(', ')}`);
            
            // 如果是我的回合，等待一下然后出牌
            if (currentUser === mySeat && myCards.length > 0) {
                setTimeout(() => {
                    const cardToPlay = myCards[0]; // 简单策略：出第一张牌
                    console.log(`\n🎴 轮到我出牌，打出: ${cardToPlay}`);
                    ws.send(JSON.stringify({
                        type: 'play_card',
                        card: cardToPlay
                    }));
                }, 1000);
            }
        }
        
        // 发牌
        if (msg.type === 'deal_cards') {
            console.log(`\n🃏 发牌: 牌=${msg.card}, 当前玩家=座位${msg.currentUser}`);
            if (msg.currentUser === mySeat) {
                myCards.push(msg.card);
                console.log(`   我的手牌更新: ${myCards.join(', ')}`);
                
                // 如果是我的回合，等待一下然后出牌
                setTimeout(() => {
                    if (myCards.length > 0) {
                        const cardToPlay = myCards[0];
                        console.log(`\n🎴 轮到我出牌，打出: ${cardToPlay}`);
                        ws.send(JSON.stringify({
                            type: 'play_card',
                            card: cardToPlay
                        }));
                    }
                }, 1000);
            }
        }
        
        // 出牌通知
        if (msg.type === 'player_play_card') {
            console.log(`\n🎴 玩家出牌: 座位${msg.seat} 打出 牌${msg.card}`);
            if (msg.seat === mySeat) {
                // 从手牌中移除
                const index = myCards.indexOf(msg.card);
                if (index > -1) {
                    myCards.splice(index, 1);
                    console.log(`   我的手牌更新: ${myCards.join(', ')}`);
                }
            }
        }
        
        // 你的回合
        if (msg.type === 'your_turn') {
            console.log(`\n⏰ 轮到你了！`);
            if (myCards.length > 0) {
                setTimeout(() => {
                    const cardToPlay = myCards[0];
                    console.log(`\n🎴 出牌: ${cardToPlay}`);
                    ws.send(JSON.stringify({
                        type: 'play_card',
                        card: cardToPlay
                    }));
                }, 1000);
            }
        }
        
        // 询问动作
        if (msg.type === 'ask_action') {
            console.log(`\n❓ 询问动作: 动作掩码=${msg.actionMask}, 牌=${msg.actionCard}`);
            // 简单策略：选择"过"
            setTimeout(() => {
                console.log(`\n✅ 选择动作: GUO (过)`);
                ws.send(JSON.stringify({
                    type: 'choose_action',
                    action: 'GUO',
                    card: msg.actionCard || 0
                }));
            }, 1000);
        }
        
        // 操作结果
        if (msg.type === 'action_result') {
            console.log(`\n✅ 操作结果: 玩家${msg.operateUser} 执行操作${msg.operateCode}，牌=${msg.operateCard}`);
        }
        
        // 游戏结束
        if (msg.type === 'round_result') {
            console.log(`\n🏁 ========== 游戏结束 ==========`);
            console.log(`   胡牌玩家: 座位${msg.huUser}`);
            if (msg.scores) {
                msg.scores.forEach((s, i) => {
                    console.log(`   玩家${i} (座位${s.seat}): 得分=${s.score}`);
                });
            }
            console.log('================================\n');
        }
        
        // 错误
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
    process.exit(0);
});

// 保持连接
console.log('⏳ 等待游戏开始...（按 Ctrl+C 退出）\n');
