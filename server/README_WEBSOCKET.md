# WebSocket 服务器使用说明

> 本文档说明如何使用 WebSocket 版本的服务器。

---

## 快速开始

### 1. 编译

```bash
cd server
mkdir build && cd build
cmake ..
make
```

编译后会生成 `mahjong_server_ws` 可执行文件。

### 2. 运行

```bash
./mahjong_server_ws
```

服务器将在 `127.0.0.1:5555` 端口监听 WebSocket 连接。

### 3. 测试连接

使用浏览器控制台：

```javascript
const ws = new WebSocket('ws://127.0.0.1:5555');
ws.onmessage = (e) => console.log('收到:', e.data);
```

或使用 Cocos 客户端连接到 `ws://127.0.0.1:5555`。

---

## 技术实现

### WebSocket 协议支持

- ✅ HTTP 握手升级（解析 `Sec-WebSocket-Key`，生成 `Sec-WebSocket-Accept`）
- ✅ 文本帧编码/解码
- ✅ 支持标准 payload length（0-125、126、127）
- ✅ 处理客户端 masked 帧

### 依赖

- **OpenSSL**：用于 SHA1 哈希（握手时需要）
  - macOS: `brew install openssl`
  - Linux: `apt-get install libssl-dev`

### 限制

- ⚠️ 当前版本是单线程阻塞，一次只能处理一个客户端
- ⚠️ 仅支持文本帧，不支持二进制帧
- ⚠️ 未实现 ping/pong 心跳机制

### 后续改进

- [ ] 多线程或异步 I/O 支持多客户端
- [ ] 实现 ping/pong 心跳
- [ ] 支持二进制帧
- [ ] 添加连接超时和自动断开机制

---

## 与 TCP 版本的对比

| 特性 | TCP 版本 | WebSocket 版本 |
|------|----------|---------------|
| 协议 | 纯 TCP 文本 | WebSocket |
| 客户端兼容 | 仅 TCP 客户端 | WebSocket 客户端（如 Cocos） |
| 依赖 | 无 | OpenSSL |
| 多客户端 | 否 | 否（当前版本） |
| 消息帧 | 按行分隔 | WebSocket 帧格式 |

**推荐使用 WebSocket 版本**，因为：
- 与 Cocos2d-x 客户端直接兼容
- 支持标准的 WebSocket 协议
- 消息边界清晰（帧格式）

---

## 故障排查

### 编译错误：找不到 OpenSSL

**现象**：
```
CMake Error: Could not find OpenSSL
```

**解决**：
1. 安装 OpenSSL（见上方"依赖"部分）
2. 如果已安装但 CMake 找不到，设置环境变量：
   ```bash
   export OPENSSL_ROOT_DIR=/usr/local/opt/openssl  # macOS
   ```

### 连接失败

**检查**：
1. 确认服务器已启动：`lsof -i :5555`
2. 确认运行的是 `mahjong_server_ws`，不是 `mahjong_server`
3. 检查防火墙设置

### 握手失败

**现象**：服务器日志显示"未找到 Sec-WebSocket-Key"

**原因**：客户端发送的不是 WebSocket 握手请求

**解决**：确保客户端使用 WebSocket 协议连接，不是普通 TCP
