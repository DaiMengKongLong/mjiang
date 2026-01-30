# 开始使用指南

> 欢迎使用麻将联机改造项目！本指南将帮助您快速开始。

---

## 🎯 项目状态

**当前状态**：✅ **基础架构已完成，可以开始测试和使用**

**已完成功能**：
- ✅ WebSocket 服务器（多线程支持）
- ✅ 客户端网络模块
- ✅ 房间管理
- ✅ 协议处理
- ✅ 测试工具
- ✅ 便捷脚本

**待完成功能**：
- ⏳ 接入真实麻将逻辑
- ⏳ 完善其他功能

---

## 🚀 快速开始（3 步）

### 步骤 1：启动服务器

打开终端，运行：

```bash
./scripts/start_server.sh
```

**预期输出**：
```
==========================================
麻将服务器启动脚本
==========================================

🚀 启动服务器...
服务器地址: ws://127.0.0.1:5555
按 Ctrl+C 停止服务器
==========================================
```

**说明**：
- 脚本会自动检查并编译服务器（如果未编译）
- 脚本会自动检查端口占用情况
- 服务器监听在 `127.0.0.1:5555`

---

### 步骤 2：运行测试客户端

**打开另一个终端**，运行：

```bash
# 单客户端测试
./scripts/test_client.sh

# 或运行多客户端测试（4个玩家）
./scripts/test_multi.sh
```

**预期输出**（单客户端）：
```
==========================================
测试客户端启动脚本
==========================================

✅ 连接成功！

📤 发送加入房间消息: {...}
📨 收到消息: {"type":"room_info",...}
✅ 加入房间成功！
```

**说明**：
- 脚本会自动检查 Node.js 和 ws 模块
- 脚本会自动检查服务器是否运行
- 测试客户端会自动响应服务器消息

---

### 步骤 3：查看结果

**服务器端**会显示：
- 客户端连接信息
- 收到的消息
- 房间状态变化

**客户端端**会显示：
- 连接状态
- 收到的所有消息
- 自动发送的操作

---

## 📚 下一步

### 1. 系统化测试

按照 `TEST_VERIFICATION.md` 进行完整的测试验证：
- 单客户端连接测试
- 多客户端连接测试
- 消息收发测试
- 房间管理测试
- 错误处理测试

### 2. 接入游戏逻辑

参考 `server/GAME_ENGINE_INTEGRATION.md` 接入真实麻将逻辑：
- 接入 `GameEngine`
- 接入 `GameLogic`
- 实现游戏流程

### 3. 集成到 Cocos 项目

参考 `client/GameLayer_Integration_Guide.md` 将网络模块集成到 Cocos 项目。

---

## 🔧 常见问题

### 问题 1：脚本无法执行

**错误**：`Permission denied`

**解决**：
```bash
chmod +x scripts/*.sh
```

### 问题 2：服务器启动失败

**可能原因**：
- 端口被占用
- OpenSSL 未安装
- 编译错误

**解决**：
- 查看 `BUILD_AND_TEST.md` 中的"常见问题排查"
- 查看 `dev_log.md` 中的"潜在错误点"

### 问题 3：测试客户端连接失败

**可能原因**：
- 服务器未启动
- Node.js 未安装
- ws 模块未安装

**解决**：
- 确保服务器已启动
- 安装 Node.js：https://nodejs.org/
- 运行 `cd test && npm install ws`

---

## 📖 相关文档

### 快速参考
- **快速开始**：[QUICKSTART.md](QUICKSTART.md) - 5 分钟快速上手
- **测试验证**：[TEST_VERIFICATION.md](TEST_VERIFICATION.md) - 完整测试流程
- **下一步行动**：[NEXT_STEPS.md](NEXT_STEPS.md) - 开发建议

### 核心文档
- **协议文档**：[protocol.md](protocol.md) - 通信协议定义
- **开发日志**：[dev_log.md](dev_log.md) - 操作记录和错误点
- **项目状态**：[PROJECT_STATUS.md](PROJECT_STATUS.md) - 完成情况

### 开发指南
- **游戏逻辑接入**：[server/GAME_ENGINE_INTEGRATION.md](server/GAME_ENGINE_INTEGRATION.md)
- **接入检查清单**：[server/INTEGRATION_CHECKLIST.md](server/INTEGRATION_CHECKLIST.md)
- **UI 集成指南**：[client/GameLayer_Integration_Guide.md](client/GameLayer_Integration_Guide.md)

### 项目文档
- **项目交付**：[PROJECT_DELIVERY.md](PROJECT_DELIVERY.md) - 完整交付清单
- **完成报告**：[COMPLETION_REPORT.md](COMPLETION_REPORT.md) - 详细完成情况
- **文档索引**：[DOCS_INDEX.md](DOCS_INDEX.md) - 快速查找所有文档

---

## 💡 提示

1. **首次使用**：
   - 建议先运行单客户端测试，熟悉基本流程
   - 然后运行多客户端测试，验证多线程功能

2. **开发调试**：
   - 查看服务器日志了解连接和消息处理情况
   - 查看测试客户端输出了解消息收发情况

3. **遇到问题**：
   - 先查看 `dev_log.md` 中的"潜在错误点"
   - 再查看相关文档的"问题排查"章节
   - 最后查看 `TEST_VERIFICATION.md` 中的"常见问题排查"

---

## 🎉 开始使用

现在您可以：

1. **立即测试**：
   ```bash
   ./scripts/start_server.sh
   # 在另一个终端
   ./scripts/test_client.sh
   ```

2. **查看文档**：
   - 阅读 `QUICKSTART.md` 了解更多细节
   - 阅读 `TEST_VERIFICATION.md` 进行系统化测试

3. **开始开发**：
   - 参考 `NEXT_STEPS.md` 选择下一步任务
   - 参考 `server/GAME_ENGINE_INTEGRATION.md` 接入游戏逻辑

---

**祝您使用愉快！** 🚀

如有问题，请查看相关文档或参考 `dev_log.md` 中的记录。
