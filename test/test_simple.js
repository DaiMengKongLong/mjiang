// 简单的测试客户端 - 快速测试连接
const WebSocket = require('ws');

const playerId = process.argv[2] || 'player_001';
const nickname = process.argv[3] || '测试玩家';
const roomId = process.argv[4] || 'test_room_001';

console.log('🚀 连接到服务器 ws://127.0.0.1:5555');
const ws = new WebSocket('ws://127.0.0.1:5555');

ws.on('open', () => {
    console.log('✅ 连接成功！');
    console.log(`📤 发送加入房间请求: ${roomId}`);
    
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
        console.log('📨 收到消息:', JSON.stringify(msg, null, 2));
        
        // 如果是房间信息，显示玩家列表
        if (msg.type === 'room_info') {
            console.log('\n📋 房间信息:');
            console.log(`   房间ID: ${msg.roomId}`);
            console.log(`   房间状态: ${msg.state}`);
            console.log(`   玩家数量: ${msg.players ? msg.players.length : 0}`);
            if (msg.players) {
                msg.players.forEach((p, i) => {
                    console.log(`   玩家${i + 1}: ${p.nickname} (${p.playerId}) - 座位${p.seat}`);
                });
            }
        }
        
        // 如果是发牌消息
        if (msg.type === 'deal_cards') {
            console.log(`\n🃏 收到 ${msg.cards ? msg.cards.length : 0} 张牌`);
        }
        
        // 如果是你的回合
        if (msg.type === 'your_turn') {
            console.log('\n⏰ 轮到你了！');
        }
        
        // 如果是错误
        if (msg.type === 'error') {
            console.log(`\n❌ 错误: ${msg.message}`);
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

// 5秒后自动关闭
setTimeout(() => {
    console.log('\n⏱️  测试完成，关闭连接...');
    ws.close();
}, 5000);
