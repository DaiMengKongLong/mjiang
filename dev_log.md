## 联机麻将改造开发日志（记录操作与错误）

> 目标：在原单机麻将项目基础上，增加联机对战能力。  
> 要求：**详细记录开发过程中的关键决策、踩坑与错误解决办法，避免以后重复犯错。**

---

### 2026-01-30 初始化

1. **创建协议文档**
   - 操作：
     - 新建 `protocol.md`，约定了客户端与 C++ 服务器之间的 JSON 协议，包括：
       - C2S：`join_room`、`play_card`、`choose_action`。
       - S2C：`room_info`、`deal_cards`、`your_turn`、`player_play_card`、`ask_action`、`round_result`、`error`。
   - 目的：
     - 让客户端和服务器可以并行开发，减少对接期的沟通成本。
   - 潜在错误点：
     - **字段名不一致**：如 `playerId` vs `playerID`，会导致反序列化失败但错误信息不明显。
       - 预防：客户端与服务器实现前，统一参考 `protocol.md`，所有修改先改文档再改代码。

2. **创建最简 C++ 服务器骨架**
   - 操作：
     - 新建目录 `server/`，添加：
       - `CMakeLists.txt`：定义目标 `mahjong_server`，包含 `main.cpp`、`Room.cpp`、`NetPlayer.cpp`。
       - `src/main.cpp`：当前仅输出启动日志，占位未来的网络监听逻辑。
       - `src/Room.h/.cpp`：房间类 `Room`，支持 `addPlayer` 与 `startGameMock`（后续挂接 `GameEngine`）。
       - `src/NetPlayer.h/.cpp`：网络玩家代理类 `NetPlayer`，目前只保存 `playerId` 和 `seat`。
   - 目的：
     - 先把“工程骨架”和基本对象关系搭好，后续接入具体网络库（asio 等）和麻将引擎时不需要再大改结构。
   - 潜在错误点：
     - **过早绑定具体网络库**：一开始就深度绑定某个网络库，后面如果改技术选型会代价很大。
       - 预防：当前版本仅在 `main.cpp` 中打印日志，不引入实际网络依赖，等协议与房间逻辑稳定后再选库。

3. **准备客户端改造计划**
   - 操作计划（尚未全部完成）：
     - 在 `server/` 目录下创建最小 C++ 服务器工程：
       - `main.cpp`：负责监听端口、接受连接、基础消息分发。
       - `Room.h/.cpp`：管理一个麻将房间，后续接入 `GameEngine`。
       - `NetPlayer.h/.cpp`：实现玩家的网络代理，后续实现 `IPlayer` 接口。
     - 在文档中整理 Cocos2d-x 客户端改造方案：
       - `NetClient`：封装 WebSocket/TCP。
       - `NetGameController`：按协议解析消息并驱动 `GameLayer`。
   - 潜在错误点：
     - 一开始就把完整麻将逻辑和网络层混在一起开发，调试难度极大。
       - 预防：先用“假数据联机流程”验证协议和 UI，再逐步接入真实 `GameEngine`。

4. **为测试联机流程实现最简 TCP 服务器（假数据版）**
   - 操作：
     - 修改 `server/src/main.cpp`：
       - 使用 BSD socket 在本机 `5555` 端口启动阻塞式 TCP 监听；
       - 接受一个客户端连接后，发送三条 JSON 测试消息（与 `protocol.md` 中的 `room_info`、`deal_cards`、`your_turn` 对应），每条以换行符结尾；
       - 循环按行读取客户端发来的文本并打印到控制台，不做 JSON 解析。
   - 目的：
     - 为后续 Cocos 客户端联机改造提供一个“最小可用”的服务器端；客户端可以连上本机端口，直接用这三条假数据驱动 UI。
   - 潜在错误点：
     - **忘记在客户端按行读取**：如果客户端没有按行（以 `\n` 为分隔）读取，而是一次性读固定字节，可能导致消息边界错乱。
       - 预防：在客户端实现中同样采用“按行读取/解析 JSON 文本”的方式，或在日志中打印原始报文确认。
     - **端口被占用或防火墙阻止**：导致客户端无法连接到 `5555` 端口。
       - 预防：在 macOS 上使用 `lsof -i :5555` 检查端口占用，必要时更换端口或调整防火墙设置。

5. **创建 Cocos 客户端网络模块（NetClient + NetGameController）**
   - 操作：
     - 新建目录 `client/`，添加：
       - `NetClient.h/.cpp`：基于 Cocos2d-x `network::WebSocket` 的单例网络客户端封装。
         - 提供 `connect(url)`、`sendJson(text)`、`close()` 接口。
         - 暴露回调：`onMessage`（收到消息）、`onConnected`、`onDisconnected`、`onError`。
       - `NetGameController.h/.cpp`：游戏网络控制器。
         - `onRawMessage(jsonText)`：解析 JSON 的 `type` 字段，分发到对应的处理函数。
         - 处理函数：`handleRoomInfo`、`handleDealCards`、`handleYourTurn`、`handlePlayerPlayCard`、`handleAskAction`、`handleRoundResult`、`handleError`。
         - 发送接口：`sendJoinRoom`、`sendPlayCard`、`sendChooseAction`。
         - 内部使用简单的字符串解析工具 `JsonHelper`（实际项目中应替换为 rapidjson）。
   - 目的：
     - 让 `GameLayer` 可以通过 `NetGameController` 与服务器通信，而不需要直接操作网络连接。
     - 所有协议解析逻辑集中在 `NetGameController`，便于维护和调试。
   - 潜在错误点：
     - **JSON 解析库选择不当**：当前使用简单的字符串解析，对于嵌套数组（如 `room_info` 中的 `players`）支持不完整。
       - 预防：实际集成到 Cocos 项目时，应使用 rapidjson 或 nlohmann/json 等专业库，确保能正确解析所有 `protocol.md` 中定义的字段。
     - **WebSocket 回调线程问题**：`NetClient::onMessage` 回调可能在非主线程执行，直接更新 UI 会导致崩溃。
       - 预防：在 `NetGameController::onRawMessage` 中，如果检测到非主线程，应使用 `Director::getInstance()->getScheduler()->performFunctionInCocosThread` 将 UI 更新投递到主线程。
     - **GameLayer 接口不匹配**：`NetGameController` 调用的 `GameLayer` 方法（如 `onRoomInfo`、`onDealCards`）如果不存在或签名不一致，会导致编译错误。
       - 预防：在集成前，先确认 `GameLayer` 中已实现这些回调方法，或根据实际项目结构调整 `NetGameController` 的调用方式。

6. **创建测试客户端示例和编译指南**
   - 操作：
     - 新建 `client/TestClientExample.h/.cpp`：独立的测试场景类，用于快速验证网络连接和协议解析。
       - `TestGameLayer`：简单的测试类，实现所有回调接口，只打印日志不更新 UI。
       - `TestClientScene`：Cocos 测试场景，包含连接按钮和日志显示，可以直接运行测试。
     - 新建 `BUILD_AND_TEST.md`：详细的编译与测试指南文档。
       - 服务器端编译步骤（CMake + make）
       - 客户端集成到 Cocos2d-x 项目的步骤
       - 联机测试流程（启动服务器 → 运行客户端 → 观察日志）
       - 常见问题排查（连接失败、解析失败、UI 崩溃、协议不匹配）
       - 下一步开发建议（分阶段任务清单）
       - 调试技巧（日志、抓包、JSON 验证、分步测试）
   - 目的：
     - 让开发者能够快速验证网络模块是否正常工作，而不需要先完整集成到原项目。
     - 提供清晰的测试流程和问题排查指南，减少调试时间。
   - 潜在错误点：
     - **服务器协议不匹配**：当前服务器使用 TCP 发送文本，但 `NetClient` 使用 WebSocket。
       - 现象：客户端无法连接，或连接后无法正常通信。
       - 预防：需要统一协议。有两个选择：
         1. 修改服务器支持 WebSocket（推荐，WebSocket 更适合游戏，支持双向通信和消息帧）
         2. 修改 `NetClient` 使用 TCP Socket（需要处理消息边界，如按行分隔或长度前缀）
       - 解决方案：在 `BUILD_AND_TEST.md` 中已记录此问题，建议优先修改服务器支持 WebSocket。
     - **测试场景无法直接使用**：`TestClientScene` 中 `NetGameController` 需要 `GameLayer` 实例，但测试场景中使用的是 `TestGameLayer`。
       - 现象：编译错误或运行时回调无法正确调用。
       - 预防：需要创建一个适配器类，或者修改 `NetGameController` 使其接受回调函数接口而非具体类。
       - 临时方案：在 `TestClientScene` 中，可以手动解析 JSON 并调用 `TestGameLayer` 的方法，或创建一个适配器类。

7. **实现 WebSocket 服务器（解决协议不匹配问题）**
   - 操作：
     - 新建 `server/src/WebSocketServer.h/.cpp`：简单的 WebSocket 服务器实现。
       - 实现 HTTP 握手升级为 WebSocket（解析 `Sec-WebSocket-Key`，生成 `Sec-WebSocket-Accept`）。
       - 实现 WebSocket 帧的编码/解码（支持文本帧，处理 mask、payload length 等）。
       - 使用 OpenSSL 的 SHA1 和自定义 Base64 编码实现握手。
     - 新建 `server/src/main_websocket.cpp`：WebSocket 版本的服务器主程序。
       - 使用 `WebSocketServer` 类启动服务器。
       - 在客户端连接时发送测试消息（`room_info`、`deal_cards`、`your_turn`）。
       - 接收并打印客户端发送的消息。
     - 更新 `server/CMakeLists.txt`：
       - 添加新的可执行文件 `mahjong_server_ws`（WebSocket 版本）。
       - 链接 OpenSSL 库（用于 SHA1 哈希）。
       - 保留原有的 `mahjong_server`（TCP 版本）用于对比测试。
   - 目的：
     - 解决服务器（TCP）与客户端（WebSocket）协议不匹配的问题。
     - 使服务器能够直接与 Cocos2d-x 的 `NetClient`（WebSocket）通信。
   - 潜在错误点：
     - **OpenSSL 依赖**：WebSocket 握手需要 SHA1 哈希，依赖 OpenSSL 库。
       - 现象：编译时找不到 OpenSSL，或链接失败。
       - 预防：确保系统已安装 OpenSSL（macOS: `brew install openssl`，Linux: `apt-get install libssl-dev`）。
       - 解决方案：在 `CMakeLists.txt` 中使用 `find_package(OpenSSL REQUIRED)`，如果找不到会给出明确错误提示。
     - **WebSocket 帧解析错误**：如果客户端发送的帧格式不符合标准，可能导致解析失败。
       - 现象：服务器无法正确读取客户端消息，或连接意外断开。
       - 预防：严格按照 WebSocket RFC 6455 标准实现帧解析，处理各种 payload length 情况（0-125、126、127）。
       - 调试：打印原始帧数据，使用 Wireshark 抓包对比。
     - **多客户端支持**：当前实现是单线程阻塞，一次只能处理一个客户端。
       - 现象：第二个客户端连接时，第一个客户端可能无法正常通信。
       - 预防：后续需要改为多线程或异步 I/O（如使用 `select`/`epoll` 或 `asio`）。
       - 临时方案：当前版本主要用于测试，单客户端足够验证协议是否正常工作。

8. **完善服务器端消息处理（解析客户端消息并实现房间管理）**
   - 操作：
     - 新建 `server/src/MessageHandler.h/.cpp`：消息处理器类。
       - 实现简单的 JSON 解析工具 `JsonParser`（提取字符串、整数、检查字段存在）。
       - 处理客户端发送的消息：`join_room`、`play_card`、`choose_action`。
       - 维护客户端与房间/玩家的映射关系（`clients_` map）。
       - 实现错误响应（`sendError`）和房间信息响应（`sendRoomInfo`）。
     - 更新 `server/src/main_websocket.cpp`：
       - 集成 `MessageHandler`，将客户端消息转发给处理器。
       - 实现简单的房间管理（使用 `std::map` 存储房间，按需创建）。
       - 移除自动发送测试消息的逻辑，改为等待客户端发送 `join_room`。
     - 更新 `server/src/Room.h`：
       - 添加 `getPlayerCount()` 方法，用于检查房间是否已满。
     - 更新 `server/CMakeLists.txt`：
       - 将 `MessageHandler.cpp` 添加到 WebSocket 服务器编译列表。
   - 目的：
     - 让服务器能够解析和处理客户端发送的协议消息。
     - 实现基本的房间管理功能（创建房间、加入房间、分配座位）。
     - 为后续接入真实麻将逻辑打下基础。
   - 潜在错误点：
     - **JSON 解析不完整**：当前使用简单的字符串解析，对嵌套对象、数组支持不完整。
       - 现象：解析复杂 JSON（如 `room_info` 中的 `players` 数组）可能失败。
       - 预防：实际项目中应使用 rapidjson 或 nlohmann/json 等专业库。
       - 临时方案：当前版本只解析简单的单层 JSON，复杂结构后续替换库时再完善。
     - **房间管理线程安全**：当前使用 `std::map` 存储房间，多线程访问可能有问题。
       - 现象：多个客户端同时创建/加入房间时可能出现数据竞争。
       - 预防：当前版本是单线程阻塞，暂时安全；后续改为多线程时需要加锁。
     - **客户端断开后未清理**：`clients_` map 中可能残留断开的客户端信息。
       - 现象：客户端断开后，再次连接可能复用旧的映射关系。
       - 预防：在 `onDisconnect` 回调中清理 `clients_` 映射（需要在 `MessageHandler` 中暴露清理接口）。

9. **重构 JSON 解析：创建 JsonHelper 封装类（为后续集成 rapidjson 做准备）**
   - 操作：
     - 新建 `server/src/JsonHelper.h/.cpp`：JSON 解析辅助类。
       - 封装所有 JSON 解析操作（`getString`、`getInt`、`getIntArray`、`getStringArray`、`buildJson`）。
       - 当前使用简单字符串解析作为临时方案。
       - 设计为易于替换：后续只需修改 `JsonHelper.cpp` 即可切换到 rapidjson。
     - 更新 `server/src/MessageHandler.h/.cpp`：
       - 移除内联的 `JsonParser` 命名空间。
       - 改为使用 `JsonHelper` 命名空间。
       - 所有 JSON 解析调用统一通过 `JsonHelper`。
     - 更新 `server/CMakeLists.txt`：
       - 将 `JsonHelper.cpp` 添加到编译列表。
     - 新建 `server/RAPIDJSON_SETUP.md`：
       - 详细说明如何集成 rapidjson（git submodule、直接下载、系统安装三种方法）。
       - 提供完整的 rapidjson 替换代码示例。
       - 说明使用 rapidjson 的优势和注意事项。
   - 目的：
     - 将 JSON 解析逻辑集中管理，便于后续替换为专业库。
     - 为支持复杂 JSON（嵌套对象、数组）做准备。
     - 提高代码可维护性（所有 JSON 操作都在一个地方）。
   - 潜在错误点：
     - **当前仍使用简单解析**：`JsonHelper` 当前实现仍然是简单字符串解析，对嵌套结构支持不完整。
       - 现象：解析 `room_info` 中的 `players` 数组时可能失败。
       - 预防：按照 `RAPIDJSON_SETUP.md` 中的说明集成 rapidjson。
       - 临时方案：当前版本只解析简单的单层 JSON，复杂结构等集成 rapidjson 后再完善。
     - **rapidjson 集成问题**：如果使用 git submodule，需要确保子模块正确初始化。
       - 现象：编译时找不到 rapidjson 头文件。
       - 预防：运行 `git submodule update --init --recursive` 确保子模块已下载。
       - 解决方案：检查 `third_party/rapidjson` 目录是否存在，如果不存在则按照 `RAPIDJSON_SETUP.md` 重新添加。

10. **完善房间管理功能（房间状态、玩家信息、离开处理）**
   - 操作：
     - 更新 `server/src/Room.h/.cpp`：
       - 添加房间状态枚举 `RoomState`（WAITING、PLAYING、FINISHED）。
       - 添加 `getPlayers()`、`getPlayerBySeat()`、`getPlayerById()` 方法。
       - 改进 `addPlayer()`：检查房间状态、是否已满、玩家是否已存在，返回 bool 表示成功/失败。
       - 添加 `removePlayer()` 和 `removePlayerBySeat()` 方法。
       - 添加 `finishGame()` 方法。
       - 使用 `playersBySeat_` map 快速查找玩家。
     - 更新 `server/src/NetPlayer.h`：
       - 添加 `nickname_` 字段和 `setNickname()`/`getNickname()` 方法。
     - 更新 `server/src/MessageHandler.h/.cpp`：
       - 完善 `sendRoomInfo()`：发送所有玩家的完整信息（seat、playerId、nickname）。
       - 添加 `sendRoomInfoToAll()`：向房间内所有玩家广播房间信息。
       - 添加 `cleanupClient()` 公开方法：处理客户端断开，从房间移除玩家并通知其他玩家。
       - 更新 `handleJoinRoom()`：使用 Room 的 `addPlayer()` 返回值，设置玩家 nickname，调用 `sendRoomInfoToAll()`。
     - 更新 `server/src/main_websocket.cpp`：
       - 在 `onDisconnect` 回调中调用 `messageHandler.cleanupClient()`。
   - 目的：
     - 实现完整的房间管理功能（加入、离开、状态管理）。
     - 确保所有玩家都能收到房间信息更新。
     - 正确处理客户端断开连接。
   - 潜在错误点：
     - **座位号分配问题**：当前按加入顺序分配座位（0-3），如果玩家离开后重新加入，座位号可能不连续。
       - 现象：房间中有玩家离开后，新玩家加入时座位号可能跳过（如：0, 1, 3）。
       - 预防：当前实现是简化版，后续可以改进为"优先使用空座位"的逻辑。
       - 临时方案：当前版本主要用于测试，座位号不连续不影响功能。
     - **房间状态检查不完整**：`addPlayer()` 只检查 `WAITING` 状态，但游戏开始后玩家离开再重新加入可能有问题。
       - 现象：游戏进行中，如果玩家断开重连，可能无法重新加入。
       - 预防：后续可以添加"重连"逻辑，允许玩家在游戏进行中重新加入（需要保存游戏状态）。
     - **广播消息时客户端已断开**：`sendRoomInfoToAll()` 向所有客户端发送消息时，如果某个客户端已断开，`sendText()` 可能失败。
       - 现象：服务器日志可能出现发送失败的错误。
       - 预防：在 `sendText()` 中检查连接状态，或捕获异常。
       - 临时方案：当前版本是单线程阻塞，客户端断开会在 `onDisconnect` 中处理，但发送时可能仍有竞态条件。

11. **实现多线程支持（支持多客户端同时连接）**
   - 操作：
     - 更新 `server/src/WebSocketServer.h/.cpp`：
       - 添加 `std::thread`、`std::mutex`、`std::atomic` 支持。
       - 将 `running_` 改为 `std::atomic<bool>` 确保线程安全。
       - 添加 `clientThreads_` map 和 `threadsMutex_` 管理客户端线程。
       - 将 `run()` 方法改为：主线程只负责 accept 新连接，每个客户端连接后创建新线程处理。
       - 添加 `handleClient()` 方法：在独立线程中处理单个客户端的消息循环。
       - 在 `sendFrame()` 中添加互斥锁保护（虽然 send 本身是线程安全的，但确保帧完整性）。
       - 在 `stop()` 中等待所有客户端线程结束。
     - 更新 `server/src/MessageHandler.h/.cpp`：
       - 添加 `clientsMutex_` 保护 `clients_` map 的访问。
       - 在所有访问 `clients_` 的地方添加 `std::lock_guard`。
       - 更新 `sendRoomInfoToAll()`：在锁保护下遍历客户端并发送消息。
     - 更新 `server/src/Room.h/.cpp`：
       - 添加 `mutable std::mutex mutex_` 保护房间数据。
       - 在所有修改房间数据的方法中添加 `std::lock_guard`（`addPlayer`、`removePlayer`、`startGameMock`、`finishGame`）。
       - 在所有读取房间数据的方法中添加 `std::lock_guard`（`getPlayerCount`、`getPlayers`、`getPlayerBySeat`、`getPlayerById`）。
       - 将 `getPlayers()` 改为返回副本而非引用，避免持有锁时返回引用导致的问题。
   - 目的：
     - 支持多个客户端同时连接和交互。
     - 让服务器能够真正支持多人在线游戏。
     - 为后续接入真实麻将逻辑打下基础。
   - 潜在错误点：
     - **线程分离问题**：使用 `detach()` 分离线程，可能导致主程序退出时线程仍在运行。
       - 现象：服务器退出时，客户端线程可能仍在运行，导致资源泄露。
       - 预防：在 `stop()` 中等待所有线程结束，但使用 `detach()` 后无法 `join()`。
       - 解决方案：改为使用 `joinable()` 检查，或改用线程池管理。
     - **死锁风险**：如果多个方法需要获取多个锁，可能出现死锁。
       - 现象：程序卡死，无响应。
       - 预防：确保所有锁的获取顺序一致（如：先获取 `clientsMutex_`，再获取 `room->mutex_`）。
       - 当前实现：每个方法只获取一个锁，暂时安全。
     - **sendText 线程安全**：虽然 `sendFrame()` 中添加了锁，但 `sendText()` 直接调用 `sendFrame()`，可能在不同线程中同时调用。
       - 现象：多个线程同时向同一客户端发送消息时，可能出现数据混乱。
       - 预防：`sendFrame()` 中的锁已经保护了发送操作。
       - 注意：如果同一客户端的多个线程同时发送，锁会串行化发送操作，可能影响性能。

12. **创建麻将逻辑接入指南**
   - 操作：
     - 新建 `server/GAME_ENGINE_INTEGRATION.md`：详细的麻将逻辑接入指南。
       - 说明如何将原项目的 `GameEngine`、`GameLogic`、`IPlayer`、`AIPlayer` 等类集成到服务器。
       - 提供架构设计对比（单机 vs 联机）。
       - 提供分步骤的接入说明（复制文件、修改接口、集成到 Room、更新 MessageHandler）。
       - 说明需要调整的地方（接口适配、依赖关系、编译配置）。
       - 提供常见问题解答。
     - 新建 `server/INTEGRATION_CHECKLIST.md`：接入检查清单。
       - 按阶段列出所有需要完成的步骤（准备、代码集成、测试、问题排查）。
       - 每个步骤都有复选框，便于跟踪进度。
       - 包含完成标准，明确接入完成的条件。
   - 目的：
     - 为后续接入真实麻将逻辑提供详细的指导。
     - 确保接入过程不遗漏关键步骤。
     - 记录可能遇到的问题和解决方案。
   - 潜在错误点：
     - **接口不匹配**：原项目的 `GameEngine` 接口可能与我们的设计不完全匹配。
       - 现象：编译错误或运行时错误。
       - 预防：接入前仔细分析原项目的接口定义，必要时创建适配器类。
       - 解决方案：根据实际情况调整接口或创建适配层。
     - **Cocos2d-x 依赖**：原项目可能依赖 Cocos2d-x 的某些类。
       - 现象：编译时找不到某些类或方法。
       - 预防：接入前检查所有依赖，列出需要替换的类。
       - 解决方案：使用标准 C++ 库替代（如 `std::shared_ptr`、`std::vector`）。
     - **事件回调不匹配**：原项目的事件回调接口可能与我们的协议消息格式不匹配。
       - 现象：客户端收到的消息格式不正确。
       - 预防：仔细对比原项目的回调接口和 `protocol.md` 中的消息格式。
       - 解决方案：在 `NetPlayer` 的事件处理方法中，将原接口的数据转换为协议消息格式。

13. **创建自动化测试工具**
   - 操作：
     - 新建 `test/test_client.js`：Node.js 测试客户端。
       - 连接到 WebSocket 服务器。
       - 发送 `join_room` 消息。
       - 接收并显示所有服务器消息（`room_info`、`deal_cards`、`your_turn`、`player_play_card`、`ask_action`、`round_result`、`error`）。
       - 自动响应 `your_turn`（自动出第一张牌）和 `ask_action`（自动选择"过"），用于快速测试。
       - 支持命令行参数：`node test_client.js [playerId] [nickname] [roomId]`。
       - 支持环境变量 `SERVER_URL` 指定服务器地址。
     - 新建 `test/test_multi_clients.sh`：多客户端测试脚本。
       - 同时启动 4 个测试客户端，模拟 4 个玩家加入同一房间。
       - 自动生成唯一的房间ID（使用时间戳）。
       - 每个客户端间隔 0.5 秒启动，避免连接冲突。
     - 新建 `test/package.json`：Node.js 依赖配置。
       - 定义 `ws` 依赖（WebSocket 客户端库）。
       - 提供 `npm test` 和 `npm run test:multi` 脚本。
     - 新建 `test/README.md`：测试工具说明文档。
       - 文件说明和使用方法。
       - 快速测试步骤。
       - 测试场景说明。
       - 问题排查指南。
     - 更新 `README.md`：添加测试工具说明。
   - 目的：
     - 提供便捷的测试工具，无需手动编写测试代码。
     - 支持快速验证服务器功能是否正常。
     - 支持多客户端测试，验证多线程和房间管理功能。
   - 潜在错误点：
     - **Node.js 未安装**：测试客户端需要 Node.js 环境。
       - 现象：运行 `node test_client.js` 时提示命令不存在。
       - 预防：在 `test/README.md` 中说明需要安装 Node.js。
       - 解决方案：安装 Node.js（https://nodejs.org/）或使用浏览器控制台测试。
     - **ws 模块未安装**：首次使用需要安装依赖。
       - 现象：运行 `node test_client.js` 时提示找不到 `ws` 模块。
       - 预防：在 `test/README.md` 中说明需要运行 `npm install ws`。
       - 解决方案：在 `test/` 目录下运行 `npm install`。
     - **多个客户端同时连接时端口冲突**：虽然服务器支持多线程，但测试脚本中客户端启动间隔太短可能导致问题。
       - 现象：某些客户端连接失败。
       - 预防：在 `test_multi_clients.sh` 中每个客户端间隔 0.5 秒启动。
       - 解决方案：如果仍有问题，可以增加间隔时间或使用不同的测试方法。

14. **修复编译错误并验证服务器编译**
   - 操作：
     - 新建 `.gitignore`：忽略构建目录、编译产物、IDE 文件、Node.js 依赖等。
     - 修复 `server/src/JsonHelper.h/.cpp` 编译错误：
       - 在 `JsonHelper.h` 中添加 `#include <map>`。
       - 在 `JsonHelper.cpp` 中添加 `#include <map>`。
       - 原因：`buildJson()` 函数使用了 `std::map`，但未包含头文件。
     - 验证服务器编译：
       - 运行 `cmake ..` 配置成功，找到 OpenSSL 库。
       - 运行 `make` 编译成功，生成 `mahjong_server` 和 `mahjong_server_ws` 可执行文件。
   - 目的：
     - 确保项目可以正常编译运行。
     - 为后续测试做准备。
   - 潜在错误点：
     - **缺少头文件包含**：使用 STL 容器（如 `std::map`、`std::vector`）时忘记包含对应头文件。
       - 现象：编译错误，提示找不到类型定义。
       - 预防：使用 STL 容器时，确保包含对应的头文件（`<map>`、`<vector>`、`<string>` 等）。
       - 解决方案：在文件开头添加 `#include <map>` 等头文件。

15. **创建测试验证指南和文档索引**
   - 操作：
     - 新建 `TEST_VERIFICATION.md`：完整的测试验证指南。
       - 测试前准备检查清单。
       - 6 个测试用例（服务器启动、单客户端连接、多客户端连接、游戏流程、错误处理、玩家离开）。
       - 每个测试用例包含步骤、预期结果、验证点。
       - 测试结果记录模板。
       - 常见问题排查指南。
       - 测试完成标准。
     - 更新 `README.md`：添加测试验证指南链接。
     - 更新 `PROJECT_STATUS.md`：标记编译验证通过。
   - 目的：
     - 提供系统化的测试流程，确保所有功能正常工作。
     - 帮助开发者快速定位和解决问题。
     - 建立测试标准，便于后续维护。
   - 潜在错误点：
     - **测试不完整**：只测试了部分功能，遗漏了边界情况。
       - 现象：某些场景下功能异常，但测试时未发现。
       - 预防：按照 `TEST_VERIFICATION.md` 逐一测试所有用例，包括错误场景。
       - 解决方案：补充遗漏的测试用例，更新测试文档。
     - **测试环境不一致**：不同开发者的测试环境不同，导致测试结果不一致。
       - 现象：某些功能在 A 的环境正常，在 B 的环境异常。
       - 预防：在测试文档中明确环境要求（操作系统、编译器版本、依赖库版本等）。
       - 解决方案：统一测试环境，或提供 Docker 容器。

16. **创建便捷启动和测试脚本**
   - 操作：
     - 新建 `scripts/start_server.sh`：服务器启动脚本。
       - 自动检查并编译服务器（如果未编译）。
       - 检查端口占用情况，提示是否终止占用进程。
       - 启动 WebSocket 服务器。
       - 提供友好的输出和错误提示。
     - 新建 `scripts/test_client.sh`：单客户端测试脚本。
       - 自动检查 Node.js 和 ws 模块。
       - 检查服务器是否运行。
       - 支持命令行参数（playerId、nickname、roomId）。
       - 自动安装依赖（如果缺失）。
     - 新建 `scripts/test_multi.sh`：多客户端测试脚本。
       - 同时启动 4 个测试客户端。
       - 每个客户端的日志保存到 `/tmp/mahjong_client_N.log`。
       - 支持 Ctrl+C 优雅退出。
     - 新建 `scripts/README.md`：脚本使用说明。
     - 更新 `README.md` 和 `QUICKSTART.md`：添加便捷脚本的使用说明。
   - 目的：
     - 简化启动和测试流程，提高开发效率。
     - 减少手动操作步骤，降低出错概率。
     - 提供统一的测试入口。
   - 潜在错误点：
     - **脚本权限问题**：脚本没有执行权限。
       - 现象：运行脚本时提示 "Permission denied"。
       - 预防：在 `scripts/README.md` 中说明需要 `chmod +x scripts/*.sh`。
       - 解决方案：运行 `chmod +x scripts/*.sh` 添加执行权限。
     - **路径问题**：脚本在不同目录运行时可能找不到文件。
       - 现象：脚本提示找不到文件或目录。
       - 预防：使用绝对路径或 `cd` 到脚本所在目录。
       - 解决方案：脚本中使用 `$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)` 获取脚本目录的绝对路径。
     - **端口检查失败**：`lsof` 命令在某些系统上可能不可用。
       - 现象：端口检查失败，但服务器仍能启动。
       - 预防：使用 `command -v lsof` 检查命令是否存在。
       - 解决方案：如果 `lsof` 不可用，跳过端口检查或使用其他方法。

17. **创建项目交付文档**
   - 操作：
     - 新建 `PROJECT_DELIVERY.md`：完整的项目交付文档。
       - 交付内容清单（核心代码、文档体系、配置文件）。
       - 功能清单（已完成和待完成）。
       - 项目状态（当前阶段和下一阶段）。
       - 代码统计（服务器端、客户端、测试工具、文档）。
       - 快速开始指南。
       - 使用说明（开发环境要求、编译说明、运行说明）。
       - 技术栈说明。
       - 文档导航。
       - 质量保证（代码质量、文档质量、测试覆盖）。
       - 学习价值说明。
       - 参考资源。
       - 支持信息（问题排查、开发建议）。
       - 项目总结和下一步建议。
     - 更新 `README.md`：添加项目交付文档链接。
     - 更新 `FINAL_SUMMARY.md`：添加项目交付文档链接。
     - 更新 `DOCS_INDEX.md`：添加项目交付文档到文档索引。
   - 目的：
     - 提供完整的项目交付清单，便于项目交接和验收。
     - 清晰展示项目完成情况和下一步计划。
     - 帮助新开发者快速了解项目全貌。
   - 潜在错误点：
     - **交付内容遗漏**：某些文件或功能未包含在交付清单中。
       - 现象：交付时发现遗漏的文件或功能。
       - 预防：定期更新交付文档，确保所有新增内容都被记录。
       - 解决方案：补充遗漏的内容到交付文档。

> 后续每次修改服务器/客户端核心逻辑或踩到异常时，请在本日志补充：  
> - 【时间】  
> - 【现象】  
> - 【定位过程】  
> - 【根因】  
> - 【解决方案】  
> 并尽量附上与 `protocol.md` 的关联说明。

