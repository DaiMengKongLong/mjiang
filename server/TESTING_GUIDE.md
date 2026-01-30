# 服务器测试指南

> 本文档说明如何测试 WebSocket 服务器的消息处理功能。

---

## 快速测试

### 1. 启动服务器

```bash
cd server/build
./mahjong_server_ws
```

服务器启动后，等待客户端连接。

### 2. 使用浏览器控制台测试

打开浏览器控制台（F12），运行以下代码：

```javascript
const ws = new WebSocket('ws://127.0.0.1:5555');

ws.onopen = () => {
    console.log('连接成功');
    
    // 发送 join_room 消息
    ws.send(JSON.stringify({
        type: 'join_room',
        roomId: 'test_room_001',
        playerId: 'player_001',
        nickname: '测试玩家1'
    }));
};

ws.onmessage = (e) => {
    console.log('收到消息:', e.data);
    const msg = JSON.parse(e.data);
    console.log('解析后:', msg);
    
    // 如果收到 room_info，可以继续测试出牌
    if (msg.type === 'room_info') {
        console.log('房间信息:', msg);
    }
    
    // 如果收到 your_turn，可以测试出牌
    if (msg.type === 'your_turn') {
        console.log('轮到我出牌，发送测试出牌消息');
        ws.send(JSON.stringify({
            type: 'play_card',
            card: 23
        }));
    }
    
    // 如果收到 ask_action，可以测试选择动作
    if (msg.type === 'ask_action') {
        console.log('询问动作，选择过');
        ws.send(JSON.stringify({
            type: 'choose_action',
            action: 'GUO',
            card: msg.card
        }));
    }
};

ws.onerror = (e) => {
    console.error('错误:', e);
};

ws.onclose = () => {
    console.log('连接关闭');
};
```

### 3. 测试流程

1. **连接服务器**：运行上述代码，应该看到"连接成功"
2. **加入房间**：发送 `join_room` 消息，应该收到 `room_info` 响应
3. **等待发牌**：如果房间有 4 个玩家，会自动开始游戏并发送 `deal_cards`
4. **出牌**：收到 `your_turn` 后，发送 `play_card` 消息
5. **选择动作**：如果收到 `ask_action`，发送 `choose_action` 消息

---

## 测试用例

### 测试 1：加入房间

**发送**：
```json
{
  "type": "join_room",
  "roomId": "test_room_001",
  "playerId": "player_001",
  "nickname": "玩家1"
}
```

**预期响应**：
```json
{
  "type": "room_info",
  "roomId": "test_room_001",
  "players": [...]
}
```

### 测试 2：出牌

**发送**：
```json
{
  "type": "play_card",
  "card": 23
}
```

**预期响应**：
```json
{
  "type": "player_play_card",
  "seat": 0,
  "card": 23
}
```

### 测试 3：选择动作

**发送**：
```json
{
  "type": "choose_action",
  "action": "PENG",
  "card": 23
}
```

**预期响应**：
```json
{
  "type": "action_confirmed",
  "action": "PENG",
  "card": 23
}
```

### 测试 4：错误处理

**发送无效消息**：
```json
{
  "type": "unknown_type"
}
```

**预期响应**：
```json
{
  "type": "error",
  "code": "UNKNOWN_TYPE",
  "message": "未知的消息类型: unknown_type"
}
```

---

## 服务器日志

服务器会输出详细的日志，包括：

- 客户端连接/断开
- 收到的消息类型和内容
- 房间创建/玩家加入
- 消息处理结果

示例日志：
```
[WebSocketServer] 启动成功，监听端口 5555
[mahjong_server] WebSocket 服务器已启动，监听端口 5555
[mahjong_server] 等待客户端连接...
[mahjong_server] 客户端已连接 (fd=4)
[MessageHandler] 收到消息类型: join_room (fd=4)
[MessageHandler] 玩家加入房间: roomId=test_room_001, playerId=player_001, nickname=玩家1
[main] 创建新房间: test_room_001
[Room] player joined, room=test_room_001, current players=1
```

---

## 常见问题

### 问题 1：连接失败

**现象**：浏览器控制台显示连接失败。

**排查**：
1. 确认服务器已启动：`lsof -i :5555`
2. 确认使用的是 WebSocket 协议（`ws://`），不是 HTTP（`http://`）
3. 检查防火墙设置

### 问题 2：收到消息但解析失败

**现象**：服务器日志显示"收到消息"，但客户端没有收到响应。

**排查**：
1. 检查 JSON 格式是否正确（使用在线 JSON 验证工具）
2. 检查字段名是否与 `protocol.md` 一致（大小写敏感）
3. 查看服务器日志，看是否有错误信息

### 问题 3：房间信息不完整

**现象**：收到的 `room_info` 中 `players` 数组只有当前玩家。

**原因**：当前实现是简化版，只发送当前玩家的信息。

**解决**：后续完善时，应该发送所有玩家的信息。

---

## 下一步

完成基础测试后，可以：

1. 测试多个客户端同时连接（需要多线程支持）
2. 测试完整的游戏流程（需要接入真实麻将逻辑）
3. 测试错误处理和边界情况
4. 性能测试（大量消息处理）
