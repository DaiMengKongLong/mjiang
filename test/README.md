# 测试工具说明

> 本目录包含用于测试麻将服务器的工具和脚本。

---

## 📋 文件说明

### `test_client.js`
Node.js 测试客户端，用于连接服务器并测试基本功能。

**功能**：
- 连接到 WebSocket 服务器
- 发送 `join_room` 消息
- 接收并显示服务器消息
- 自动响应 `your_turn` 和 `ask_action`（测试用）

**使用方法**：
```bash
# 安装依赖（首次使用）
npm install ws

# 运行测试客户端
node test_client.js [playerId] [nickname] [roomId]

# 示例
node test_client.js player_001 玩家1 test_room
```

**环境变量**：
- `SERVER_URL`：服务器地址（默认：`ws://127.0.0.1:5555`）

### `test_multi_clients.sh`
Shell 脚本，同时启动 4 个测试客户端，模拟 4 个玩家。

**使用方法**：
```bash
# 确保有执行权限
chmod +x test_multi_clients.sh

# 运行脚本
./test_multi_clients.sh
```

**注意**：
- 需要先安装 Node.js
- 需要先启动服务器：`./mahjong_server_ws`

---

## 🚀 快速测试

### 1. 启动服务器

```bash
cd server/build
./mahjong_server_ws
```

### 2. 运行单个测试客户端

```bash
cd test
npm install ws  # 首次使用
node test_client.js
```

### 3. 运行多客户端测试

```bash
cd test
chmod +x test_multi_clients.sh
./test_multi_clients.sh
```

---

## 📊 测试场景

### 场景 1：单客户端连接
- 测试连接、加入房间、接收消息

### 场景 2：多客户端连接
- 测试 4 个客户端同时连接
- 测试房间信息广播
- 测试游戏开始（4 个玩家）

### 场景 3：游戏流程
- 测试发牌、出牌、吃碰杠胡、结算

---

## 🔧 自定义测试

可以修改 `test_client.js` 来自定义测试行为：

- 修改自动出牌逻辑
- 修改动作选择逻辑
- 添加更多测试场景

---

## 📝 注意事项

1. **确保服务器已启动**：测试前先启动服务器
2. **检查端口**：确保服务器监听在 `5555` 端口
3. **Node.js 版本**：建议使用 Node.js 12 或更高版本

---

## 🐛 问题排查

### 问题：连接失败

**检查**：
1. 服务器是否已启动
2. 端口是否正确（默认 5555）
3. 防火墙是否阻止连接

### 问题：npm install 失败

**解决**：
```bash
# 使用国内镜像
npm install ws --registry=https://registry.npmmirror.com
```

---

## 📚 相关文档

- 服务器测试指南：`../server/TESTING_GUIDE.md`
- 多线程测试指南：`../server/MULTITHREAD_TEST.md`
- 快速开始：`../QUICKSTART.md`
