# 测试验证指南

> 本文档提供完整的测试验证流程，确保所有功能正常工作。

---

## 📋 测试前准备

### 1. 确认环境

- [ ] CMake 已安装（`cmake --version`）
- [ ] C++ 编译器已安装（`g++ --version` 或 `clang++ --version`）
- [ ] OpenSSL 已安装（`openssl version`）
- [ ] Node.js 已安装（`node --version`，用于测试客户端）

### 2. 编译服务器

```bash
cd server
mkdir -p build && cd build
cmake ..
make
```

**预期结果**：
- CMake 配置成功，找到 OpenSSL
- 编译成功，生成 `mahjong_server_ws` 可执行文件

---

## 🧪 测试用例

### 测试 1：服务器启动

**步骤**：
```bash
cd server/build
./mahjong_server_ws
```

**预期结果**：
```
[WebSocketServer] 启动成功，监听端口 5555
[mahjong_server] WebSocket 服务器已启动，监听端口 5555
[mahjong_server] 等待客户端连接...
```

**验证点**：
- [ ] 服务器正常启动
- [ ] 无错误信息
- [ ] 监听在 5555 端口

---

### 测试 2：单客户端连接

**步骤 1**：启动服务器（在一个终端）
```bash
cd server/build
./mahjong_server_ws
```

**步骤 2**：运行测试客户端（在另一个终端）
```bash
cd test
npm install ws
node test_client.js player_001 玩家1 test_room
```

**预期结果**：

**服务器端**：
```
[WebSocketServer] 新客户端连接: 127.0.0.1:xxxxx (fd=X)
[WebSocketServer] WebSocket 握手成功
[mahjong_server] 客户端已连接 (fd=X)
[WebSocketServer] 已为客户端创建处理线程 (fd=X)
[MessageHandler] 收到消息类型: join_room (fd=X)
[MessageHandler] 玩家加入房间: roomId=test_room, playerId=player_001, nickname=玩家1
[main] 创建新房间: test_room
[Room] 玩家加入: room=test_room, playerId=player_001, seat=0, current players=1
[MessageHandler] 向房间 test_room 的所有玩家发送房间信息
```

**客户端端**：
```
✅ 连接成功！
📤 发送加入房间消息: {...}
📨 收到消息: {"type":"room_info",...}
✅ 加入房间成功！
```

**验证点**：
- [ ] 客户端能成功连接
- [ ] 服务器收到 `join_room` 消息
- [ ] 客户端收到 `room_info` 响应
- [ ] 房间信息包含正确的玩家数据

---

### 测试 3：多客户端连接

**步骤 1**：启动服务器
```bash
cd server/build
./mahjong_server_ws
```

**步骤 2**：运行多客户端测试脚本
```bash
cd test
./test_multi_clients.sh
```

**预期结果**：
- 4 个客户端都能成功连接
- 每个客户端都收到 `room_info`，包含所有 4 个玩家
- 当第 4 个玩家加入时，房间自动开始游戏
- 所有玩家都收到 `deal_cards` 消息

**验证点**：
- [ ] 所有客户端都能连接
- [ ] 房间信息正确广播给所有玩家
- [ ] 游戏能正常开始（4 个玩家时）
- [ ] 所有玩家都收到发牌消息

---

### 测试 4：游戏流程（如果已接入麻将逻辑）

**步骤**：
1. 4 个玩家加入房间
2. 游戏开始，收到 `deal_cards`
3. 收到 `your_turn` 后，发送 `play_card`
4. 收到 `ask_action` 后，发送 `choose_action`
5. 游戏结束，收到 `round_result`

**验证点**：
- [ ] 发牌功能正常
- [ ] 出牌功能正常
- [ ] 吃碰杠胡功能正常
- [ ] 结算功能正常

---

### 测试 5：错误处理

**测试场景**：
1. 发送无效消息类型
2. 发送格式错误的 JSON
3. 未加入房间就发送 `play_card`

**预期结果**：
- 服务器返回 `error` 消息
- 服务器不崩溃
- 错误信息清晰

**验证点**：
- [ ] 错误处理正常
- [ ] 错误消息格式正确
- [ ] 服务器稳定运行

---

### 测试 6：玩家离开

**步骤**：
1. 多个玩家加入房间
2. 某个客户端断开连接（Ctrl+C）
3. 其他玩家应该收到更新后的 `room_info`

**预期结果**：
- 服务器检测到客户端断开
- 从房间中移除玩家
- 其他玩家收到更新后的房间信息

**验证点**：
- [ ] 断开检测正常
- [ ] 房间信息正确更新
- [ ] 其他玩家收到通知

---

## 📊 测试结果记录

### 测试日期：___________

| 测试用例 | 状态 | 备注 |
|---------|------|------|
| 服务器启动 | ⬜ 通过 / ⬜ 失败 | |
| 单客户端连接 | ⬜ 通过 / ⬜ 失败 | |
| 多客户端连接 | ⬜ 通过 / ⬜ 失败 | |
| 游戏流程 | ⬜ 通过 / ⬜ 失败 | |
| 错误处理 | ⬜ 通过 / ⬜ 失败 | |
| 玩家离开 | ⬜ 通过 / ⬜ 失败 | |

### 发现的问题

1. **问题描述**：
   - 现象：
   - 复现步骤：
   - 解决方案：

---

## 🔧 常见问题排查

### 问题 1：服务器启动失败

**可能原因**：
- 端口被占用
- OpenSSL 未安装
- 编译错误

**排查步骤**：
```bash
# 检查端口占用
lsof -i :5555

# 检查 OpenSSL
openssl version

# 重新编译
cd server/build
make clean
cmake ..
make
```

### 问题 2：客户端连接失败

**可能原因**：
- 服务器未启动
- 防火墙阻止
- 端口错误

**排查步骤**：
```bash
# 检查服务器是否运行
ps aux | grep mahjong_server_ws

# 检查端口监听
lsof -i :5555

# 测试连接
telnet 127.0.0.1 5555
```

### 问题 3：消息解析失败

**可能原因**：
- JSON 格式错误
- 字段名不匹配
- 编码问题

**排查步骤**：
- 打印原始 JSON 消息
- 使用在线 JSON 验证工具检查格式
- 对比 `protocol.md` 确认字段名

---

## ✅ 测试完成标准

所有测试通过的标准：
- [ ] 服务器能正常启动和运行
- [ ] 单客户端能正常连接和通信
- [ ] 多客户端能同时连接
- [ ] 房间管理功能正常
- [ ] 消息收发正常
- [ ] 错误处理正常
- [ ] 玩家离开处理正常

---

## 📝 测试报告模板

```
测试日期：2026-01-30
测试人员：___________
测试环境：macOS / Linux / Windows

测试结果：
- 服务器启动：✅ / ❌
- 单客户端连接：✅ / ❌
- 多客户端连接：✅ / ❌
- 游戏流程：✅ / ❌（如果已接入）
- 错误处理：✅ / ❌
- 玩家离开：✅ / ❌

发现的问题：
1. ...

建议：
1. ...
```

---

## 📚 相关文档

- 快速开始：`QUICKSTART.md`
- 编译测试：`BUILD_AND_TEST.md`
- 服务器测试：`server/TESTING_GUIDE.md`
- 多线程测试：`server/MULTITHREAD_TEST.md`
- 测试工具：`test/README.md`

---

**使用说明**：按照测试用例逐一验证，记录测试结果和发现的问题。
