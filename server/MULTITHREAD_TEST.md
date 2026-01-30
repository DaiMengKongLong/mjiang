# 多线程服务器测试指南

> 本文档说明如何测试多线程服务器的多客户端支持功能。

---

## 测试多客户端连接

### 方法 1：使用多个浏览器标签页

1. **启动服务器**：
```bash
cd server/build
./mahjong_server_ws
```

2. **打开多个浏览器标签页**，每个标签页运行以下代码：

```javascript
// 标签页 1 - 玩家1
const ws1 = new WebSocket('ws://127.0.0.1:5555');
ws1.onopen = () => {
    console.log('玩家1连接成功');
    ws1.send(JSON.stringify({
        type: 'join_room',
        roomId: 'test_room_001',
        playerId: 'player_001',
        nickname: '玩家1'
    }));
};
ws1.onmessage = (e) => {
    const msg = JSON.parse(e.data);
    console.log('玩家1收到:', msg);
};

// 标签页 2 - 玩家2
const ws2 = new WebSocket('ws://127.0.0.1:5555');
ws2.onopen = () => {
    console.log('玩家2连接成功');
    ws2.send(JSON.stringify({
        type: 'join_room',
        roomId: 'test_room_001',
        playerId: 'player_002',
        nickname: '玩家2'
    }));
};
ws2.onmessage = (e) => {
    const msg = JSON.parse(e.data);
    console.log('玩家2收到:', msg);
};

// 标签页 3 - 玩家3
// 标签页 4 - 玩家4
// ... 类似
```

### 方法 2：使用 Node.js 脚本

创建 `test_multi_client.js`：

```javascript
const WebSocket = require('ws');

const players = [
    { id: 'player_001', nickname: '玩家1' },
    { id: 'player_002', nickname: '玩家2' },
    { id: 'player_003', nickname: '玩家3' },
    { id: 'player_004', nickname: '玩家4' }
];

const roomId = 'test_room_001';

players.forEach((player, index) => {
    setTimeout(() => {
        const ws = new WebSocket('ws://127.0.0.1:5555');
        
        ws.on('open', () => {
            console.log(`[${player.nickname}] 连接成功`);
            ws.send(JSON.stringify({
                type: 'join_room',
                roomId: roomId,
                playerId: player.id,
                nickname: player.nickname
            }));
        });
        
        ws.on('message', (data) => {
            const msg = JSON.parse(data.toString());
            console.log(`[${player.nickname}] 收到:`, msg);
        });
        
        ws.on('error', (error) => {
            console.error(`[${player.nickname}] 错误:`, error);
        });
        
        ws.on('close', () => {
            console.log(`[${player.nickname}] 连接关闭`);
        });
    }, index * 500);  // 每个客户端间隔 500ms 连接
});
```

运行：
```bash
npm install ws
node test_multi_client.js
```

---

## 预期行为

### 1. 多个客户端同时连接

- 每个客户端应该能够成功连接
- 服务器日志应该显示多个客户端连接信息
- 每个客户端应该收到自己的连接确认

### 2. 多个玩家加入同一房间

- 每个玩家发送 `join_room` 后，应该收到 `room_info`
- `room_info` 中的 `players` 数组应该包含所有已加入的玩家
- 当第 4 个玩家加入时，房间应该自动开始游戏

### 3. 房间信息广播

- 当新玩家加入时，房间内所有玩家都应该收到更新后的 `room_info`
- `players` 数组应该包含所有玩家的完整信息（seat、playerId、nickname）

### 4. 玩家离开处理

- 当某个客户端断开连接时，其他玩家应该收到更新后的 `room_info`
- 断开连接的玩家应该从 `players` 数组中移除

---

## 服务器日志示例

```
[WebSocketServer] 启动成功，监听端口 5555
[mahjong_server] WebSocket 服务器已启动，监听端口 5555
[mahjong_server] 等待客户端连接...
[WebSocketServer] 新客户端连接: 127.0.0.1:54321 (fd=4)
[WebSocketServer] WebSocket 握手成功
[mahjong_server] 客户端已连接 (fd=4)
[WebSocketServer] 已为客户端创建处理线程 (fd=4)
[MessageHandler] 收到消息类型: join_room (fd=4)
[MessageHandler] 玩家加入房间: roomId=test_room_001, playerId=player_001, nickname=玩家1
[main] 创建新房间: test_room_001
[Room] 玩家加入: room=test_room_001, playerId=player_001, seat=0, current players=1
[MessageHandler] 向房间 test_room_001 的所有玩家发送房间信息
[WebSocketServer] 新客户端连接: 127.0.0.1:54322 (fd=5)
[WebSocketServer] WebSocket 握手成功
[mahjong_server] 客户端已连接 (fd=5)
[WebSocketServer] 已为客户端创建处理线程 (fd=5)
...
```

---

## 常见问题

### 问题 1：客户端连接失败

**现象**：第二个客户端无法连接。

**排查**：
1. 检查服务器日志，看是否有错误信息
2. 确认服务器是否正常启动
3. 检查防火墙设置

### 问题 2：房间信息不完整

**现象**：某个客户端收到的 `room_info` 中 `players` 数组不完整。

**排查**：
1. 检查服务器日志，确认所有玩家是否都成功加入
2. 检查 `sendRoomInfoToAll()` 是否正确遍历所有客户端
3. 检查线程同步是否正确（是否有竞态条件）

### 问题 3：服务器崩溃

**现象**：多个客户端连接后，服务器崩溃。

**排查**：
1. 检查是否有未捕获的异常
2. 检查内存访问是否越界
3. 检查互斥锁是否正确使用（是否有死锁）

---

## 性能测试

### 测试并发连接数

使用压力测试工具（如 `websocat` 或自定义脚本）测试服务器能同时处理多少个客户端连接。

### 测试消息吞吐量

测试服务器每秒能处理多少条消息。

---

## 下一步

完成多线程测试后，可以：

1. 测试完整的游戏流程（4 个玩家加入 → 开始游戏 → 出牌 → 结算）
2. 测试异常情况（客户端突然断开、网络延迟等）
3. 性能优化（如果需要）
