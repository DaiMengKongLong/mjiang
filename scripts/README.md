# 便捷脚本说明

> 本目录包含用于快速启动和测试的便捷脚本。

---

## 📋 脚本列表

### `start_server.sh`
启动麻将服务器。

**功能**：
- 自动检查并编译服务器（如果未编译）
- 检查端口占用情况
- 启动 WebSocket 服务器

**使用方法**：
```bash
./scripts/start_server.sh
```

**示例输出**：
```
==========================================
麻将服务器启动脚本
==========================================

🚀 启动服务器...
服务器地址: ws://127.0.0.1:5555
按 Ctrl+C 停止服务器
==========================================
```

---

### `test_client.sh`
启动单个测试客户端。

**功能**：
- 自动检查 Node.js 和 ws 模块
- 检查服务器是否运行
- 启动测试客户端

**使用方法**：
```bash
# 使用默认参数
./scripts/test_client.sh

# 指定参数
./scripts/test_client.sh player_001 玩家1 test_room
```

**参数**：
- `playerId`：玩家ID（可选，默认自动生成）
- `nickname`：昵称（可选，默认自动生成）
- `roomId`：房间ID（可选，默认自动生成）

---

### `test_multi.sh`
启动 4 个测试客户端，模拟多玩家游戏。

**功能**：
- 自动检查 Node.js 和 ws 模块
- 检查服务器是否运行
- 同时启动 4 个测试客户端
- 每个客户端的日志保存到 `/tmp/mahjong_client_N.log`

**使用方法**：
```bash
./scripts/test_multi.sh
```

**查看日志**：
```bash
# 查看客户端 1 的日志
tail -f /tmp/mahjong_client_1.log

# 查看所有客户端日志
tail -f /tmp/mahjong_client_*.log
```

---

## 🚀 快速开始

### 1. 启动服务器

```bash
./scripts/start_server.sh
```

### 2. 在另一个终端运行测试客户端

**单客户端测试**：
```bash
./scripts/test_client.sh
```

**多客户端测试**：
```bash
./scripts/test_multi.sh
```

---

## 📝 注意事项

1. **首次使用**：
   - 脚本会自动检查并安装依赖
   - 如果服务器未编译，会自动编译

2. **端口占用**：
   - `start_server.sh` 会自动检测端口 5555 是否被占用
   - 如果被占用，会提示是否终止占用进程

3. **服务器检查**：
   - `test_client.sh` 和 `test_multi.sh` 会检查服务器是否运行
   - 如果未运行，会提示警告

4. **日志文件**：
   - 多客户端测试的日志保存在 `/tmp/mahjong_client_N.log`
   - 可以使用 `tail -f` 实时查看日志

---

## 🔧 故障排查

### 问题：脚本无法执行

**解决**：
```bash
chmod +x scripts/*.sh
```

### 问题：找不到 Node.js

**解决**：
- 安装 Node.js：https://nodejs.org/
- 或使用系统包管理器安装

### 问题：端口被占用

**解决**：
- 使用 `lsof -i :5555` 查看占用进程
- 使用 `kill -9 <PID>` 终止进程
- 或修改服务器端口（需要修改代码）

---

## 📚 相关文档

- 快速开始：`../QUICKSTART.md`
- 测试验证：`../TEST_VERIFICATION.md`
- 测试工具：`../test/README.md`
