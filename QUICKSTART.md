# 快速开始指南

> 5 分钟快速上手麻将联机服务器

---

## 前置要求

- **操作系统**：macOS / Linux / Windows
- **编译器**：支持 C++11（g++、clang++、MSVC）
- **CMake**：3.10 或更高版本
- **OpenSSL**：用于 WebSocket 握手（macOS: `brew install openssl`，Linux: `apt-get install libssl-dev`）

---

## 步骤 1：编译服务器

```bash
# 进入服务器目录
cd server

# 创建构建目录
mkdir build
cd build

# 生成 Makefile
cmake ..

# 编译
make
```

编译成功后，会生成 `mahjong_server_ws` 可执行文件。

---

## 步骤 2：启动服务器

```bash
./mahjong_server_ws
```

服务器启动后，会看到：
```
[WebSocketServer] 启动成功，监听端口 5555
[mahjong_server] WebSocket 服务器已启动，监听端口 5555
[mahjong_server] 等待客户端连接...
```

---

## 步骤 3：测试连接

### 方法 1：使用便捷脚本（最简单）

```bash
# 单客户端测试
./scripts/test_client.sh

# 多客户端测试（4个玩家）
./scripts/test_multi.sh
```

### 方法 2：使用 Node.js 测试客户端

```bash
# 进入测试目录
cd test

# 安装依赖（首次使用）
npm install ws

# 运行测试客户端
node test_client.js player_001 玩家1 test_room
```

测试客户端会自动：
- 连接到服务器
- 发送加入房间消息
- 接收并显示所有服务器消息
- 自动响应出牌和动作选择（用于快速测试）

### 方法 2：使用浏览器控制台

打开浏览器控制台（F12），运行以下代码：

```javascript
// 连接到服务器
const ws = new WebSocket('ws://127.0.0.1:5555');

// 连接成功
ws.onopen = () => {
    console.log('✅ 连接成功！');
    
    // 发送加入房间消息
    ws.send(JSON.stringify({
        type: 'join_room',
        roomId: 'test_room_001',
        playerId: 'player_001',
        nickname: '测试玩家1'
    }));
};

// 接收消息
ws.onmessage = (e) => {
    const msg = JSON.parse(e.data);
    console.log('📨 收到消息:', msg);
    
    // 如果收到 room_info，说明加入成功
    if (msg.type === 'room_info') {
        console.log('✅ 加入房间成功！');
        console.log('房间信息:', msg);
    }
    
    // 如果收到 your_turn，可以测试出牌
    if (msg.type === 'your_turn') {
        console.log('🎮 轮到你出牌了！');
        // 可以发送出牌消息
        // ws.send(JSON.stringify({ type: 'play_card', card: 23 }));
    }
};

// 错误处理
ws.onerror = (e) => {
    console.error('❌ 连接错误:', e);
};

// 连接关闭
ws.onclose = () => {
    console.log('🔌 连接已关闭');
};
```

---

## 步骤 4：测试多客户端（可选）

### 方法 1：使用测试脚本（推荐）

```bash
cd test
chmod +x test_multi_clients.sh
./test_multi_clients.sh
```

脚本会自动启动 4 个测试客户端，模拟 4 个玩家加入同一房间。

### 方法 2：使用浏览器标签页

打开多个浏览器标签页，每个标签页运行上面的代码，但修改 `playerId` 和 `nickname`：

```javascript
// 标签页 2
playerId: 'player_002', nickname: '测试玩家2'

// 标签页 3
playerId: 'player_003', nickname: '测试玩家3'

// 标签页 4
playerId: 'player_004', nickname: '测试玩家4'
```

当 4 个玩家都加入后，房间会自动开始游戏。

---

## 步骤 5：测试出牌

收到 `your_turn` 消息后，可以发送出牌消息：

```javascript
ws.send(JSON.stringify({
    type: 'play_card',
    card: 23  // 牌编号（0-33）
}));
```

---

## 常见问题

### Q: 编译失败，提示找不到 OpenSSL

**A**: 安装 OpenSSL：
- macOS: `brew install openssl`
- Linux: `apt-get install libssl-dev` 或 `yum install openssl-devel`
- Windows: 使用 vcpkg 或下载预编译库

### Q: 连接失败

**A**: 检查：
1. 服务器是否已启动
2. 端口 5555 是否被占用：`lsof -i :5555`（macOS/Linux）
3. 防火墙是否阻止了连接

### Q: 收到消息但解析失败

**A**: 当前使用简单 JSON 解析，对复杂结构支持不完整。可以：
1. 检查 JSON 格式是否正确
2. 参考 `server/RAPIDJSON_SETUP.md` 集成 rapidjson

---

## 下一步

- **测试多客户端**：
  - 使用测试脚本：`cd test && ./test_multi_clients.sh`
  - 或参考 `server/MULTITHREAD_TEST.md`
- **集成到 Cocos 项目**：参考 `client/GameLayer_Integration_Guide.md`
- **查看完整文档**：参考 `BUILD_AND_TEST.md`
- **测试工具说明**：参考 `test/README.md`

---

## 需要帮助？

- 查看 `dev_log.md` 了解已知问题和解决方案
- 查看 `PROJECT_STATUS.md` 了解项目当前状态
- 查看 `protocol.md` 了解协议详情
