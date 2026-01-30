# 文档索引

> 快速查找所有项目文档

---

## 🚀 快速开始

| 文档 | 说明 | 适合人群 |
|------|------|---------|
| [GETTING_STARTED.md](GETTING_STARTED.md) | 3 步快速开始 | 新手（推荐） |
| [QUICKSTART.md](QUICKSTART.md) | 5 分钟快速上手 | 新手 |
| [TEST_VERIFICATION.md](TEST_VERIFICATION.md) | 完整测试验证流程 | 测试人员 |
| [NEXT_STEPS.md](NEXT_STEPS.md) | 下一步开发建议 | 开发者 |

---

## 📚 核心文档

| 文档 | 说明 | 重要性 |
|------|------|--------|
| [protocol.md](protocol.md) | 客户端与服务器通信协议 | ⭐⭐⭐⭐⭐ |
| [dev_log.md](dev_log.md) | 开发日志（操作记录和错误点） | ⭐⭐⭐⭐⭐ |
| [README.md](README.md) | 项目总览和快速开始 | ⭐⭐⭐⭐⭐ |

---

## 📊 项目状态

| 文档 | 说明 |
|------|------|
| [PROJECT_STATUS.md](PROJECT_STATUS.md) | 项目完成情况和功能列表 |
| [PROJECT_DELIVERY.md](PROJECT_DELIVERY.md) | 项目交付文档（完整交付清单） |
| [COMPLETION_REPORT.md](COMPLETION_REPORT.md) | 项目完成报告（详细完成情况） |
| [FINAL_SUMMARY.md](FINAL_SUMMARY.md) | 项目完成总结 |
| [SUMMARY.md](SUMMARY.md) | 项目总结 |

---

## 🛠️ 开发指南

### 编译和测试

| 文档 | 说明 |
|------|------|
| [BUILD_AND_TEST.md](BUILD_AND_TEST.md) | 编译与测试完整指南 |

### 服务器端

| 文档 | 说明 |
|------|------|
| [server/GAME_ENGINE_INTEGRATION.md](server/GAME_ENGINE_INTEGRATION.md) | 麻将逻辑接入指南 |
| [server/INTEGRATION_CHECKLIST.md](server/INTEGRATION_CHECKLIST.md) | 接入检查清单 |
| [server/RAPIDJSON_SETUP.md](server/RAPIDJSON_SETUP.md) | RapidJSON 集成指南 |
| [server/TESTING_GUIDE.md](server/TESTING_GUIDE.md) | 服务器测试指南 |
| [server/MULTITHREAD_TEST.md](server/MULTITHREAD_TEST.md) | 多线程测试指南 |
| [server/README_WEBSOCKET.md](server/README_WEBSOCKET.md) | WebSocket 服务器说明 |

### 客户端

| 文档 | 说明 |
|------|------|
| [client/GameLayer_Integration_Guide.md](client/GameLayer_Integration_Guide.md) | GameLayer 集成指南 |
| [client_net_plan.md](client_net_plan.md) | 客户端改造方案 |

### 测试工具

| 文档 | 说明 |
|------|------|
| [test/README.md](test/README.md) | 测试工具说明 |

---

## 📖 文档分类

### 按用途分类

**入门文档**：
- `README.md` - 项目总览
- `QUICKSTART.md` - 快速开始
- `NEXT_STEPS.md` - 下一步行动

**开发文档**：
- `protocol.md` - 协议文档
- `dev_log.md` - 开发日志
- `BUILD_AND_TEST.md` - 编译测试
- `client_net_plan.md` - 客户端方案

**集成文档**：
- `server/GAME_ENGINE_INTEGRATION.md` - 游戏逻辑接入
- `server/INTEGRATION_CHECKLIST.md` - 接入清单
- `client/GameLayer_Integration_Guide.md` - UI 集成

**测试文档**：
- `TEST_VERIFICATION.md` - 测试验证
- `server/TESTING_GUIDE.md` - 服务器测试
- `server/MULTITHREAD_TEST.md` - 多线程测试
- `test/README.md` - 测试工具

**状态文档**：
- `PROJECT_STATUS.md` - 项目状态
- `FINAL_SUMMARY.md` - 完成总结

### 按阶段分类

**阶段 1：基础架构** ✅
- 协议设计：`protocol.md`
- 服务器实现：`server/` 目录
- 客户端网络模块：`client/` 目录
- 测试工具：`test/` 目录

**阶段 2：测试验证** ⏳
- 测试验证：`TEST_VERIFICATION.md`
- 快速开始：`QUICKSTART.md`

**阶段 3：游戏逻辑接入** ⏳
- 接入指南：`server/GAME_ENGINE_INTEGRATION.md`
- 接入清单：`server/INTEGRATION_CHECKLIST.md`

**阶段 4：功能完善** ⏳
- RapidJSON 集成：`server/RAPIDJSON_SETUP.md`
- UI 集成：`client/GameLayer_Integration_Guide.md`

---

## 🔍 快速查找

### 我想...

**快速上手**：
1. 阅读 `README.md` 了解项目
2. 按照 `QUICKSTART.md` 运行服务器和测试客户端
3. 查看 `TEST_VERIFICATION.md` 进行完整测试

**开始开发**：
1. 阅读 `protocol.md` 了解通信协议
2. 查看 `dev_log.md` 了解开发历史和潜在错误点
3. 参考 `NEXT_STEPS.md` 选择下一步任务

**接入游戏逻辑**：
1. 阅读 `server/GAME_ENGINE_INTEGRATION.md` 了解架构
2. 按照 `server/INTEGRATION_CHECKLIST.md` 逐步接入
3. 参考 `dev_log.md` 中的相关记录

**测试功能**：
1. 使用 `test/test_client.js` 进行单客户端测试
2. 使用 `test/test_multi_clients.sh` 进行多客户端测试
3. 按照 `TEST_VERIFICATION.md` 进行完整测试

**解决问题**：
1. 查看 `dev_log.md` 中的"潜在错误点"部分
2. 查看 `TEST_VERIFICATION.md` 中的"常见问题排查"
3. 查看相关文档的"问题排查"章节

---

## 📝 文档维护

**文档更新原则**：
- 每次添加新功能时，更新相关文档
- 发现错误时，在 `dev_log.md` 中记录
- 修改协议时，更新 `protocol.md` 并通知所有开发者

**文档版本**：
- 所有文档使用 Markdown 格式
- 重要变更在文档开头标注日期
- 重大变更在 `dev_log.md` 中记录

---

**最后更新**：2026-01-30  
**文档总数**：20+ 个文档  
**建议阅读顺序**：`README.md` → `QUICKSTART.md` → `protocol.md` → `NEXT_STEPS.md`
