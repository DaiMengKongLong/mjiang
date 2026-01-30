# 编译与测试指南

> 本文档说明如何编译服务器和客户端，并进行联机测试。

---

## 一、服务器端编译与运行

### 1. 环境要求

- **操作系统**：macOS / Linux / Windows
- **编译器**：支持 C++11 的编译器（g++、clang++、MSVC）
- **构建工具**：CMake 3.10 或更高版本
- **依赖库**：
  - **WebSocket 版本需要**：OpenSSL（用于 SHA1 哈希）
    - macOS: `brew install openssl`
    - Linux: `apt-get install libssl-dev` 或 `yum install openssl-devel`
    - Windows: 使用 vcpkg 或下载预编译库

### 2. 编译步骤

```bash
# 进入服务器目录
cd server

# 创建构建目录
mkdir build
cd build

# 生成 Makefile（macOS/Linux）或 Visual Studio 项目（Windows）
cmake ..

# 编译
make        # macOS/Linux
# 或
cmake --build . --config Release  # Windows (Visual Studio)
```

编译后会生成两个可执行文件：
- **`mahjong_server`**：TCP 版本（原始版本，用于测试）
- **`mahjong_server_ws`**：WebSocket 版本（**推荐使用，与客户端兼容**）

### 3. 运行 WebSocket 服务器（推荐）

```bash
# 运行 WebSocket 版本
./mahjong_server_ws    # macOS/Linux
# 或
.\mahjong_server_ws.exe  # Windows
```

服务器启动后，应该看到类似输出：

```
[WebSocketServer] 启动成功，监听端口 5555
[mahjong_server] WebSocket 服务器已启动，等待客户端连接...
```

此时服务器正在 `127.0.0.1:5555` 端口监听 WebSocket 连接。

### 4. 运行 TCP 服务器（仅用于对比测试）

```bash
# 运行 TCP 版本（仅用于测试，客户端使用 WebSocket 无法连接）
./mahjong_server    # macOS/Linux
# 或
.\mahjong_server.exe  # Windows
```

TCP 版本启动后，可以使用 `nc` 工具测试：

```bash
# macOS/Linux
nc 127.0.0.1 5555

# 如果连接成功，你会看到服务器发送的三条 JSON 消息
# 输入任意文本后按回车，服务器会打印出来
```

**注意**：TCP 版本与 Cocos 客户端的 WebSocket 不兼容，仅用于服务器端测试。

### 5. 测试 WebSocket 服务器

可以使用浏览器控制台或 WebSocket 测试工具：

```javascript
// 在浏览器控制台中运行
const ws = new WebSocket('ws://127.0.0.1:5555');
ws.onopen = () => console.log('连接成功');
ws.onmessage = (e) => console.log('收到消息:', e.data);
ws.onerror = (e) => console.error('错误:', e);
ws.onclose = () => console.log('连接关闭');
```

或者使用命令行工具（需要安装 `websocat`）：

```bash
websocat ws://127.0.0.1:5555
```

---

## 二、客户端集成到 Cocos2d-x 项目

### 1. 添加文件到项目

将以下文件添加到你的 Cocos2d-x 项目中：

```
client/
├── NetClient.h
├── NetClient.cpp
├── NetGameController.h
└── NetGameController.cpp
```

**注意**：如果项目使用 Visual Studio，需要在项目属性中添加这些文件。

### 2. 修改 CMakeLists.txt 或项目配置

如果使用 CMake，在 `CMakeLists.txt` 中添加：

```cmake
# 添加客户端网络模块
set(CLIENT_NET_SOURCES
    client/NetClient.cpp
    client/NetGameController.cpp
)

target_sources(your_target_name PRIVATE ${CLIENT_NET_SOURCES})
```

### 3. 在 GameLayer 中集成

参考 `client/GameLayer_Integration_Guide.md` 中的说明：

1. 在 `GameLayer.h` 中添加 `NetGameController` 成员变量
2. 在 `GameLayer::init()` 中初始化网络连接
3. 实现 `onRoomInfo`、`onDealCards` 等回调函数
4. 修改 UI 事件处理，改为通过 `NetGameController` 发送消息

### 4. 编译客户端

按照你的 Cocos2d-x 项目正常编译流程：

```bash
# 例如使用 cocos 命令行工具
cocos compile -p mac|ios|android|win32

# 或使用 Xcode / Visual Studio / Android Studio
```

---

## 三、联机测试流程

### 步骤 1：启动服务器

```bash
cd server/build
./mahjong_server
```

确认服务器已启动并监听 `5555` 端口。

### 步骤 2：运行客户端

运行你的 Cocos2d-x 客户端应用。

### 步骤 3：观察日志

**服务器端日志**应该显示：
```
[mahjong_server] 客户端已连接
[mahjong_server] 发送: {"type":"room_info",...}
[mahjong_server] 发送: {"type":"deal_cards",...}
[mahjong_server] 发送: {"type":"your_turn",...}
```

**客户端日志**（在 Xcode Console 或 Visual Studio Output 窗口）应该显示：
```
[NetClient] 尝试连接到: ws://127.0.0.1:5555
[NetClient] 连接已建立
[NetClient] 收到: {"type":"room_info",...}
[NetGameController] 收到消息类型: room_info
[GameLayer] 房间信息: roomId=test_room, ...
```

### 步骤 4：测试交互

1. **测试接收消息**：
   - 客户端应该能收到服务器的三条测试消息
   - 检查 `GameLayer` 的回调函数是否被正确调用

2. **测试发送消息**：
   - 在客户端 UI 中点击"出牌"按钮（或调用 `netController_->sendPlayCard(23)`)
   - 服务器端应该打印出收到的 JSON 文本

---

## 四、常见问题排查

### 问题 1：客户端无法连接到服务器

**现象**：客户端显示"连接失败"或超时。

**排查步骤**：
1. 确认服务器已启动：`lsof -i :5555`（macOS/Linux）或 `netstat -an | findstr 5555`（Windows）
2. 确认防火墙没有阻止 5555 端口
3. 检查服务器日志，看是否有错误信息
4. **重要**：确保运行的是 **WebSocket 版本**（`mahjong_server_ws`），而不是 TCP 版本
   - TCP 版本（`mahjong_server`）与 WebSocket 客户端不兼容

**解决方案**：
- 使用 `./mahjong_server_ws` 启动 WebSocket 服务器
- 如果编译失败，检查是否安装了 OpenSSL 库

### 问题 2：收到消息但解析失败

**现象**：客户端日志显示"收到消息"，但 `NetGameController` 没有调用对应的处理函数。

**排查步骤**：
1. 打印原始 JSON 文本，检查格式是否正确
2. 检查 `protocol.md` 中的字段名是否与服务器发送的一致
3. 检查 `JsonHelper` 是否能正确解析（当前是简化实现，对嵌套数组支持不完整）

**解决方案**：
- 替换 `NetGameController.cpp` 中的 `JsonHelper` 为 rapidjson 或 nlohmann/json
- 参考 `client/GameLayer_Integration_Guide.md` 中的说明

### 问题 3：UI 更新导致崩溃

**现象**：收到网络消息后，更新 UI 时程序崩溃。

**排查步骤**：
1. 检查是否在主线程更新 UI
2. WebSocket 回调可能在非主线程执行

**解决方案**：
- 确保所有 UI 更新都通过 `Director::getInstance()->getScheduler()->performFunctionInCocosThread` 投递到主线程
- 参考 `NetGameController.cpp` 中的示例代码

### 问题 4：服务器和客户端协议不匹配

**现象**：客户端发送的消息服务器无法识别，或服务器发送的消息客户端解析失败。

**排查步骤**：
1. 对比服务器发送的 JSON 和 `protocol.md` 中的定义
2. 检查字段名大小写（`playerId` vs `playerID`）
3. 检查 JSON 格式（是否有多余的逗号、引号等）

**解决方案**：
- 统一参考 `protocol.md`，所有修改先更新文档再改代码
- 在服务器和客户端都打印原始 JSON，便于对比

---

## 五、下一步开发建议

### 阶段 1：网络连接验证（当前阶段）

- [x] 服务器能监听端口并接受连接
- [x] 客户端能连接到服务器
- [x] 客户端能收到服务器的测试消息
- [ ] 客户端能正确解析 JSON 并调用回调函数
- [ ] 客户端能发送消息到服务器

### 阶段 2：协议完善

- [ ] 替换 `JsonHelper` 为专业 JSON 库（rapidjson）
- [ ] 完善服务器端消息处理（解析客户端发送的 `join_room`、`play_card` 等）
- [ ] 实现完整的协议消息类型（所有 `protocol.md` 中定义的消息）

### 阶段 3：接入真实麻将逻辑

- [ ] 将原项目的 `GameEngine`、`GameLogic` 复制到服务器
- [ ] 在 `Room` 中接入 `GameEngine`
- [ ] 实现 `NetPlayer` 的 `IGameEngineEventListener` 接口
- [ ] 测试完整的一局麻将流程

### 阶段 4：完善功能

- [ ] 实现房间管理（创建房间、加入房间、房间列表）
- [ ] 实现断线重连
- [ ] 实现超时托管
- [ ] 添加错误处理和用户提示

---

## 六、调试技巧

### 1. 启用详细日志

在 `NetClient.cpp` 和 `NetGameController.cpp` 中，所有关键操作都有 `CCLOG` 输出。确保在项目设置中启用了日志输出。

### 2. 使用网络抓包工具

- **macOS/Linux**：`tcpdump` 或 `Wireshark`
- **Windows**：`Wireshark`

可以抓取 TCP/WebSocket 数据包，查看实际传输的 JSON 文本。

### 3. 使用 JSON 验证工具

将服务器或客户端发送的 JSON 文本复制到在线 JSON 验证工具（如 https://jsonlint.com/），检查格式是否正确。

### 4. 分步测试

- 先测试服务器能否发送消息
- 再测试客户端能否接收消息
- 最后测试双向通信

每一步都确认无误后再进行下一步。

---

## 七、联系与反馈

如果在编译或测试过程中遇到问题，请：

1. 检查 `dev_log.md` 中是否已有类似问题的记录
2. 在 `dev_log.md` 中记录新发现的问题和解决方案
3. 对比 `protocol.md` 确认协议是否一致
