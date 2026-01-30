# 麻将逻辑接入检查清单

> 按步骤检查接入进度，确保不遗漏关键步骤。

---

## 📋 准备阶段

### 1. 获取原项目代码
- [ ] 克隆或下载原项目 [xiyoufang/mahjong](https://github.com/xiyoufang/mahjong)
- [ ] 确认原项目的目录结构
- [ ] 确认需要复制的核心类文件

### 2. 分析依赖关系
- [ ] 检查 `GameEngine` 的依赖
- [ ] 检查 `GameLogic` 的依赖
- [ ] 检查是否有 Cocos2d-x 特定依赖
- [ ] 列出需要替换或适配的类

### 3. 准备测试环境
- [ ] 准备测试用例（发牌、出牌、吃碰杠胡）
- [ ] 准备多客户端测试脚本
- [ ] 准备日志记录工具

---

## 🔧 代码集成阶段

### 4. 复制核心类
- [ ] 创建 `server/src/game/` 目录
- [ ] 复制 `GameEngine.h/.cpp`
- [ ] 复制 `GameLogic.h/.cpp`
- [ ] 复制 `IPlayer.h`
- [ ] 复制 `AIPlayer.h/.cpp`
- [ ] 复制 `AIEngine.h/.cpp`
- [ ] 复制 `GameCmd.h`
- [ ] 复制 `FvMask.h`
- [ ] 复制其他依赖文件

### 5. 移除 Cocos2d-x 依赖
- [ ] 替换 `cocos2d::Ref` → `std::shared_ptr` 或移除
- [ ] 替换 `cocos2d::Vector` → `std::vector`
- [ ] 替换 `CCLOG` → `std::cout` 或自定义日志
- [ ] 移除其他 Cocos2d-x 特定代码

### 6. 修改 NetPlayer
- [ ] 让 `NetPlayer` 继承 `IPlayer`
- [ ] 实现 `IPlayer` 接口方法
- [ ] 实现 `IGameEngineEventListener` 接口（如果存在）
- [ ] 添加 `sendMessage()` 方法
- [ ] 添加 `clientFd_` 和 `server_` 成员

### 7. 集成到 Room
- [ ] 添加 `gameEngine_` 成员
- [ ] 实现 `startGame()` 方法
- [ ] 在 `startGame()` 中创建 `GameEngine`
- [ ] 将 `NetPlayer` 注册到 `GameEngine`
- [ ] 调用 `GameEngine::startGame()`
- [ ] 更新 `startGameMock()` 调用 `startGame()`

### 8. 更新 MessageHandler
- [ ] 在 `handlePlayCard()` 中调用 `GameEngine`
- [ ] 在 `handleChooseAction()` 中调用 `GameEngine`
- [ ] 添加房间状态检查（确保在游戏中）
- [ ] 添加错误处理

### 9. 更新 CMakeLists.txt
- [ ] 添加游戏逻辑源文件到编译列表
- [ ] 检查依赖库（如果有）
- [ ] 测试编译

---

## 🧪 测试阶段

### 10. 编译测试
- [ ] 清理构建目录
- [ ] 运行 `cmake ..`
- [ ] 运行 `make`
- [ ] 检查编译错误
- [ ] 修复编译错误

### 11. 功能测试
- [ ] 测试服务器启动
- [ ] 测试客户端连接
- [ ] 测试加入房间
- [ ] 测试游戏开始（4 个玩家）
- [ ] 测试发牌（收到 `deal_cards`）
- [ ] 测试出牌（收到 `your_turn` → 发送 `play_card`）
- [ ] 测试吃碰杠胡（收到 `ask_action` → 发送 `choose_action`）
- [ ] 测试结算（收到 `round_result`）

### 12. 多客户端测试
- [ ] 测试 4 个客户端同时连接
- [ ] 测试所有玩家都能收到房间信息
- [ ] 测试所有玩家都能收到游戏状态更新
- [ ] 测试玩家出牌后，其他玩家能收到通知

### 13. AI 玩家测试（如果实现）
- [ ] 测试少于 4 个玩家时自动添加 AI
- [ ] 测试 AI 玩家能正常出牌
- [ ] 测试 AI 玩家能正常响应吃碰杠胡

---

## 🐛 问题排查

### 14. 常见问题检查
- [ ] 检查事件回调是否正确触发
- [ ] 检查消息格式是否符合协议
- [ ] 检查线程安全问题
- [ ] 检查内存泄漏
- [ ] 检查日志输出

### 15. 性能测试
- [ ] 测试多房间并发
- [ ] 测试消息处理延迟
- [ ] 测试内存使用
- [ ] 测试 CPU 使用

---

## 📝 文档更新

### 16. 更新文档
- [ ] 更新 `dev_log.md` 记录接入过程
- [ ] 更新 `PROJECT_STATUS.md` 标记功能完成
- [ ] 更新 `README.md` 说明新功能
- [ ] 记录遇到的问题和解决方案

---

## ✅ 完成标准

接入完成的标准：
- [ ] 服务器能正常编译运行
- [ ] 客户端能连接并加入房间
- [ ] 游戏能正常开始（4 个玩家）
- [ ] 发牌、出牌、吃碰杠胡功能正常
- [ ] 结算功能正常
- [ ] 多客户端测试通过
- [ ] 无明显的线程安全问题
- [ ] 无内存泄漏

---

## 📚 相关文档

- 接入指南：`GAME_ENGINE_INTEGRATION.md`
- 协议文档：`../protocol.md`
- 开发日志：`../dev_log.md`
- 测试指南：`TESTING_GUIDE.md`

---

**使用说明**：每完成一个步骤，在对应的复选框打勾 ✅，确保不遗漏任何关键步骤。
