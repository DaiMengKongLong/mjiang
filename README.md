# 麻将联机改造项目

> 基于原单机麻将项目（[xiyoufang/mahjong](https://github.com/xiyoufang/mahjong)），改造为支持联机对战的版本。

**项目状态**：✅ 基础架构已完成，可以开始测试和使用  
**立即开始**：查看 [GETTING_STARTED.md](GETTING_STARTED.md) - 3 步快速开始 ⭐  
**上传云端**：查看 [DEPLOYMENT_GUIDE.md](DEPLOYMENT_GUIDE.md) - 代码上传和服务器部署指南 🚀  
**项目就绪**：查看 [PROJECT_READY.md](PROJECT_READY.md) - 项目就绪通知  
**项目交付**：查看 [PROJECT_DELIVERY.md](PROJECT_DELIVERY.md) - 完整交付内容  
**完成报告**：查看 [COMPLETION_REPORT.md](COMPLETION_REPORT.md) - 详细完成情况

---

## 📋 项目结构

```
麻将2026-01-30/
├── protocol.md                    # 联机协议文档（客户端与服务器通信协议）
├── dev_log.md                     # 开发日志（记录操作与错误）
├── BUILD_AND_TEST.md              # 编译与测试指南
├── client_net_plan.md             # 客户端改造方案文档
│
├── server/                        # C++ 服务器端
│   ├── CMakeLists.txt
│   ├── README_WEBSOCKET.md        # WebSocket 服务器使用说明
│   ├── TESTING_GUIDE.md           # 服务器测试指南
│   └── src/
│       ├── main.cpp               # TCP 版本服务器（仅用于测试）
│       ├── main_websocket.cpp     # WebSocket 版本服务器主程序
│       ├── WebSocketServer.h/.cpp # WebSocket 服务器实现
│       ├── MessageHandler.h/.cpp  # 消息处理器（解析客户端消息）
│       ├── Room.h/.cpp            # 房间管理类
│       └── NetPlayer.h/.cpp        # 网络玩家代理类
│
├── client/                        # Cocos2d-x 客户端网络模块
│   ├── NetClient.h/.cpp           # 网络客户端封装（WebSocket）
│   ├── NetGameController.h/.cpp  # 游戏网络控制器（协议解析）
│   ├── TestClientExample.h/.cpp   # 测试客户端示例
│   └── GameLayer_Integration_Guide.md  # GameLayer 集成指南
│
├── test/                          # 测试工具
│   ├── test_client.js             # Node.js 测试客户端
│   ├── test_multi_clients.sh      # 多客户端测试脚本
│   ├── package.json               # Node.js 依赖配置
│   └── README.md                  # 测试工具说明
│
└── scripts/                       # 便捷脚本
    ├── start_server.sh            # 启动服务器脚本
    ├── test_client.sh             # 测试客户端脚本
    ├── test_multi.sh              # 多客户端测试脚本
    └── README.md                  # 脚本说明
```

---

## 🚀 快速开始

> **立即开始**：查看 [GETTING_STARTED.md](GETTING_STARTED.md) - 3 步快速开始  
> **详细指南**：查看 [QUICKSTART.md](QUICKSTART.md) - 5 分钟快速上手

### 1. 编译服务器

```bash
cd server
mkdir build && cd build
cmake ..
make
```

编译后会生成：
- **`mahjong_server_ws`**：WebSocket 版本（**推荐使用**）
- `mahjong_server`：TCP 版本（仅用于测试）

### 2. 运行服务器

```bash
./mahjong_server_ws
```

服务器将在 `127.0.0.1:5555` 端口监听 WebSocket 连接。

**注意**：需要 OpenSSL 库（macOS: `brew install openssl`，Linux: `apt-get install libssl-dev`）

### 3. 测试连接

使用浏览器控制台（F12）：
```javascript
const ws = new WebSocket('ws://127.0.0.1:5555');
ws.onopen = () => {
    ws.send(JSON.stringify({
        type: 'join_room',
        roomId: 'test_room',
        playerId: 'player_001',
        nickname: '测试玩家'
    }));
};
ws.onmessage = (e) => console.log('收到:', JSON.parse(e.data));
```

### 4. 测试连接（使用测试工具）

**方法 1：使用 Node.js 测试客户端（推荐）**
```bash
cd test
npm install ws
node test_client.js player_001 玩家1 test_room
```

**方法 2：使用浏览器控制台**
参考 `QUICKSTART.md` 中的浏览器测试代码

**方法 3：测试多客户端**
```bash
cd test
chmod +x test_multi_clients.sh
./test_multi_clients.sh
```

### 5. 集成到 Cocos 项目

1. 将 `client/NetClient.h/.cpp` 和 `client/NetGameController.h/.cpp` 添加到项目
2. 参考 `client/GameLayer_Integration_Guide.md` 集成网络模块
3. 连接到 `ws://127.0.0.1:5555` 测试

**详细步骤**：
- 快速开始：**[QUICKSTART.md](QUICKSTART.md)**
- 完整指南：**[BUILD_AND_TEST.md](BUILD_AND_TEST.md)**
- 项目状态：**[PROJECT_STATUS.md](PROJECT_STATUS.md)**
- 测试工具：**[test/README.md](test/README.md)**

---

## 📚 文档说明

> **完整文档索引**：查看 [DOCS_INDEX.md](DOCS_INDEX.md) 快速查找所有文档

### 核心文档

- **[protocol.md](protocol.md)**：客户端与服务器之间的 JSON 协议定义
  - 所有消息类型、字段说明、示例 JSON
  - **重要**：修改协议时，先更新此文档，再同步修改服务器和客户端代码

- **[dev_log.md](dev_log.md)**：开发过程记录
  - 每次关键操作的说明
  - 潜在错误点与预防措施
  - 实际遇到的问题与解决方案

### 开发指南

- **[NEXT_STEPS.md](NEXT_STEPS.md)**：下一步行动指南 ⭐
  - 当前项目状态总结
  - 按优先级排列的开发建议
  - 推荐开发流程

- **[QUICKSTART.md](QUICKSTART.md)**：快速开始指南
  - 5 分钟快速上手
  - 浏览器测试代码
  - 常见问题解答

- **[BUILD_AND_TEST.md](BUILD_AND_TEST.md)**：编译与测试指南
  - 服务器和客户端的编译步骤
  - 联机测试流程
  - 常见问题排查

- **[DEPLOYMENT_GUIDE.md](DEPLOYMENT_GUIDE.md)**：云端部署指南 🚀
  - 上传代码到 GitHub/GitLab
  - 部署服务器到云服务器（阿里云/腾讯云等）
  - Docker 部署方案
  - 部署检查清单和常见问题

- **[server/GAME_ENGINE_INTEGRATION.md](server/GAME_ENGINE_INTEGRATION.md)**：麻将逻辑接入指南
  - 如何将原项目的 `GameEngine` 集成到服务器
  - 架构设计和代码示例
  - 常见问题解答

- **[server/INTEGRATION_CHECKLIST.md](server/INTEGRATION_CHECKLIST.md)**：接入检查清单
  - 分步骤的接入检查清单
  - 便于跟踪进度

- **[client/GameLayer_Integration_Guide.md](client/GameLayer_Integration_Guide.md)**：GameLayer 集成指南
  - 需要实现的回调接口
  - 初始化代码示例
  - UI 事件处理改造示例

---

## ⚠️ 重要注意事项

### 1. 协议一致性

- 服务器和客户端必须严格按照 `protocol.md` 中的定义收发消息
- 字段名大小写敏感（`playerId` vs `playerID` 会导致解析失败）
- 修改协议时，先更新 `protocol.md`，再同步修改代码

### 2. 网络协议

- **当前状态**：服务器和客户端都使用 **WebSocket** ✅
- **服务器版本**：
  - `mahjong_server_ws`：WebSocket 版本（推荐，与客户端兼容）
  - `mahjong_server`：TCP 版本（仅用于测试，与客户端不兼容）
- **客户端**：使用 Cocos2d-x 的 `network::WebSocket`

### 3. JSON 解析库

- 当前 `NetGameController` 使用简单的字符串解析（`JsonHelper`）
- **限制**：对嵌套数组支持不完整（如 `room_info` 中的 `players`）
- **建议**：实际集成时替换为 rapidjson 或 nlohmann/json

### 4. 线程安全

- WebSocket 回调可能在非主线程执行
- 所有 UI 更新必须通过 `performFunctionInCocosThread` 投递到主线程

---

## 🔄 开发进度

### ✅ 已完成

- [x] **协议文档设计**（`protocol.md`）
- [x] **WebSocket 服务器实现**（支持 HTTP 握手升级、帧编码/解码）
- [x] **多线程支持**（每个客户端独立线程，支持多客户端同时连接）
- [x] **服务器端消息处理**（解析 `join_room`、`play_card`、`choose_action`）
- [x] **房间管理**（创建、加入、离开、状态管理、线程安全）
- [x] **客户端网络模块**（`NetClient` + `NetGameController`）
- [x] **JSON 解析封装**（`JsonHelper`，可替换为 rapidjson）
- [x] **完整文档**（协议、开发日志、测试指南等）

### 🚧 待完成

- [ ] **接入真实麻将逻辑**（`GameEngine`、`GameLogic`、AI）
- [ ] 集成 rapidjson（提高 JSON 解析可靠性）
- [ ] 实现断线重连机制
- [ ] 完善房间管理（房间列表、状态查询）
- [ ] 性能优化和压力测试

**详细状态**：查看 [PROJECT_STATUS.md](PROJECT_STATUS.md)

### 📋 待完成

- [ ] 接入真实麻将逻辑（`GameEngine`、`GameLogic`）
- [ ] 实现房间管理（创建、加入、列表）
- [ ] 实现断线重连
- [ ] 实现超时托管
- [ ] 完善错误处理和用户提示

---

## 🎯 下一步行动

**不知道从哪里开始？** 查看 **[NEXT_STEPS.md](NEXT_STEPS.md)** 获取详细的开发建议和优先级指导。

**推荐流程**：
1. ✅ **测试当前基础架构**（参考 `QUICKSTART.md` 或 `TEST_VERIFICATION.md`）
2. ⏳ 接入真实麻将逻辑（参考 `server/GAME_ENGINE_INTEGRATION.md`）
3. ⏳ 完善功能和优化

**快速测试**（使用便捷脚本）：
```bash
# 1. 启动服务器（自动编译和检查）
./scripts/start_server.sh

# 2. 在另一个终端运行测试客户端
./scripts/test_client.sh

# 或运行多客户端测试
./scripts/test_multi.sh
```

**手动测试**：
```bash
# 1. 启动服务器
cd server/build && ./mahjong_server_ws

# 2. 在另一个终端运行测试客户端
cd test && npm install ws && node test_client.js
```

---

## 🐛 问题反馈

如果在开发过程中遇到问题：

1. 先查看 `dev_log.md` 中是否已有类似问题的记录
2. 查看 `BUILD_AND_TEST.md` 中的常见问题排查
3. 在 `dev_log.md` 中记录新发现的问题和解决方案

---

## 📖 参考资源

- 原项目：[xiyoufang/mahjong](https://github.com/xiyoufang/mahjong)
- Cocos2d-x 文档：https://docs.cocos2d-x.org/
- WebSocket 协议：https://tools.ietf.org/html/rfc6455

---

## 📝 许可证

本项目基于原项目 [xiyoufang/mahjong](https://github.com/xiyoufang/mahjong) 进行改造，请遵循原项目的许可证要求。
