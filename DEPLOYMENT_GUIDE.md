# 云端部署指南

本指南包含两种云端部署方式：
1. **代码上传到 GitHub/GitLab**（代码托管）
2. **服务器部署到云服务器**（运行环境）

---

## 📦 方式一：上传代码到 GitHub/GitLab

### 1. 初始化 Git 仓库

```bash
cd /Users/ouyjay/Desktop/麻将2026-01-30

# 初始化 Git 仓库
git init

# 添加所有文件（.gitignore 会自动排除不需要的文件）
git add .

# 提交代码
git commit -m "初始提交：麻将联机项目基础架构"
```

### 2. 上传到 GitHub

#### 步骤 1：在 GitHub 创建新仓库

1. 访问 https://github.com/new
2. 填写仓库名称（如：`mahjong-online`）
3. 选择 **Public** 或 **Private**
4. **不要**勾选 "Initialize this repository with a README"（因为本地已有代码）
5. 点击 "Create repository"

#### 步骤 2：连接本地仓库到 GitHub

```bash
# 添加远程仓库（替换 YOUR_USERNAME 和 YOUR_REPO_NAME）
git remote add origin https://github.com/YOUR_USERNAME/YOUR_REPO_NAME.git

# 或者使用 SSH（推荐）
git remote add origin git@github.com:YOUR_USERNAME/YOUR_REPO_NAME.git

# 推送代码到 GitHub
git branch -M main
git push -u origin main
```

**如果使用 HTTPS，GitHub 会要求输入用户名和 Personal Access Token（不是密码）**

#### 步骤 3：后续更新代码

```bash
# 添加修改的文件
git add .

# 提交更改
git commit -m "描述你的更改"

# 推送到 GitHub
git push
```

### 3. 上传到 GitLab

#### 步骤 1：在 GitLab 创建新项目

1. 访问 https://gitlab.com/projects/new
2. 填写项目名称
3. 选择可见性级别
4. 点击 "Create project"

#### 步骤 2：连接本地仓库到 GitLab

```bash
# 添加远程仓库
git remote add origin https://gitlab.com/YOUR_USERNAME/YOUR_REPO_NAME.git

# 推送代码
git branch -M main
git push -u origin main
```

---

## 🚀 方式二：部署服务器到云服务器

### 方案 A：部署到阿里云/腾讯云/华为云

#### 1. 准备工作

**购买云服务器**：
- 推荐配置：2核4G，Ubuntu 20.04/22.04 或 CentOS 7/8
- 开放端口：5555（WebSocket 服务器端口）

**连接服务器**：
```bash
ssh root@YOUR_SERVER_IP
```

#### 2. 在服务器上安装依赖

**Ubuntu/Debian**：
```bash
# 更新系统
apt update && apt upgrade -y

# 安装编译工具
apt install -y build-essential cmake git

# 安装 OpenSSL（WebSocket 需要）
apt install -y libssl-dev

# 安装 Node.js（用于测试客户端，可选）
curl -fsSL https://deb.nodesource.com/setup_18.x | bash -
apt install -y nodejs
```

**CentOS/RHEL**：
```bash
# 更新系统
yum update -y

# 安装编译工具
yum groupinstall -y "Development Tools"
yum install -y cmake git

# 安装 OpenSSL
yum install -y openssl-devel

# 安装 Node.js（可选）
curl -fsSL https://rpm.nodesource.com/setup_18.x | bash -
yum install -y nodejs
```

#### 3. 上传代码到服务器

**方法 1：使用 Git（推荐）**

```bash
# 在服务器上克隆仓库
cd /opt
git clone https://github.com/YOUR_USERNAME/YOUR_REPO_NAME.git
cd YOUR_REPO_NAME
```

**方法 2：使用 SCP**

```bash
# 在本地执行（将整个项目打包上传）
cd /Users/ouyjay/Desktop
tar -czf mahjong.tar.gz 麻将2026-01-30
scp mahjong.tar.gz root@YOUR_SERVER_IP:/opt/

# 在服务器上解压
ssh root@YOUR_SERVER_IP
cd /opt
tar -xzf mahjong.tar.gz
cd 麻将2026-01-30
```

#### 4. 编译服务器

```bash
cd /opt/YOUR_REPO_NAME/server
mkdir build && cd build
cmake ..
make
```

#### 5. 配置防火墙

**Ubuntu（ufw）**：
```bash
ufw allow 5555/tcp
ufw reload
```

**CentOS（firewalld）**：
```bash
firewall-cmd --permanent --add-port=5555/tcp
firewall-cmd --reload
```

**云服务器控制台**：
- 在阿里云/腾讯云控制台的安全组中，添加入站规则：端口 5555，协议 TCP

#### 6. 运行服务器

**方式 1：直接运行（测试用）**
```bash
cd /opt/YOUR_REPO_NAME/server/build
./mahjong_server_ws
```

**方式 2：后台运行（推荐）**
```bash
cd /opt/YOUR_REPO_NAME/server/build
nohup ./mahjong_server_ws > server.log 2>&1 &
```

**方式 3：使用 systemd 服务（生产环境推荐）**

创建服务文件：
```bash
sudo nano /etc/systemd/system/mahjong-server.service
```

内容：
```ini
[Unit]
Description=Mahjong WebSocket Server
After=network.target

[Service]
Type=simple
User=root
WorkingDirectory=/opt/YOUR_REPO_NAME/server/build
ExecStart=/opt/YOUR_REPO_NAME/server/build/mahjong_server_ws
Restart=always
RestartSec=10
StandardOutput=journal
StandardError=journal

[Install]
WantedBy=multi-user.target
```

启动服务：
```bash
sudo systemctl daemon-reload
sudo systemctl enable mahjong-server
sudo systemctl start mahjong-server

# 查看状态
sudo systemctl status mahjong-server

# 查看日志
sudo journalctl -u mahjong-server -f
```

#### 7. 测试连接

在本地测试连接到云服务器：
```bash
# 修改测试客户端，连接到云服务器 IP
cd test
node test_client.js player_test 测试玩家 test_room
```

或者使用浏览器控制台：
```javascript
const ws = new WebSocket('ws://YOUR_SERVER_IP:5555');
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

---

### 方案 B：使用 Docker 部署（推荐）

#### 1. 创建 Dockerfile

在项目根目录创建 `Dockerfile`：

```dockerfile
FROM ubuntu:22.04

# 安装依赖
RUN apt update && apt install -y \
    build-essential \
    cmake \
    git \
    libssl-dev \
    && rm -rf /var/lib/apt/lists/*

# 设置工作目录
WORKDIR /app

# 复制代码
COPY . .

# 编译服务器
WORKDIR /app/server
RUN mkdir build && cd build && \
    cmake .. && \
    make

# 暴露端口
EXPOSE 5555

# 运行服务器
WORKDIR /app/server/build
CMD ["./mahjong_server_ws"]
```

#### 2. 构建 Docker 镜像

```bash
cd /Users/ouyjay/Desktop/麻将2026-01-30
docker build -t mahjong-server:latest .
```

#### 3. 运行容器

```bash
docker run -d \
  --name mahjong-server \
  -p 5555:5555 \
  mahjong-server:latest
```

#### 4. 查看日志

```bash
docker logs -f mahjong-server
```

#### 5. 使用 Docker Compose（可选）

创建 `docker-compose.yml`：

```yaml
version: '3.8'

services:
  mahjong-server:
    build: .
    container_name: mahjong-server
    ports:
      - "5555:5555"
    restart: unless-stopped
    volumes:
      - ./server/logs:/app/server/logs
```

运行：
```bash
docker-compose up -d
```

---

### 方案 C：使用云平台服务（Serverless/容器服务）

#### 阿里云函数计算 / 腾讯云 SCF

需要将服务器改造为 HTTP 接口（当前是 WebSocket，需要适配）

#### 阿里云容器服务 ACK / 腾讯云 TKE

使用 Kubernetes 部署 Docker 容器，支持自动扩缩容

---

## 🔧 客户端连接配置

部署到云服务器后，需要修改客户端的连接地址：

### Cocos2d-x 客户端

在 `NetClient.cpp` 中修改连接地址：

```cpp
// 本地测试
std::string url = "ws://127.0.0.1:5555";

// 云服务器（替换为实际 IP 或域名）
std::string url = "ws://YOUR_SERVER_IP:5555";

// 使用域名（推荐）
std::string url = "ws://mahjong.yourdomain.com:5555";
```

### 测试客户端

```bash
# 修改 test_client.js 中的服务器地址
# 或使用环境变量
SERVER_URL=ws://YOUR_SERVER_IP:5555 node test_client.js
```

---

## 📋 部署检查清单

- [ ] 代码已上传到 GitHub/GitLab
- [ ] 云服务器已购买并配置
- [ ] 服务器依赖已安装（cmake, openssl-dev）
- [ ] 代码已上传到服务器
- [ ] 服务器已编译成功
- [ ] 防火墙端口 5555 已开放
- [ ] 服务器已启动并运行
- [ ] 客户端可以连接到服务器
- [ ] 多客户端测试通过
- [ ] 服务器日志正常

---

## 🐛 常见问题

### 1. 连接被拒绝

**原因**：防火墙未开放端口或服务器未启动

**解决**：
- 检查服务器是否运行：`ps aux | grep mahjong_server_ws`
- 检查端口监听：`netstat -tlnp | grep 5555`
- 检查防火墙规则

### 2. 编译失败

**原因**：缺少依赖库

**解决**：
- Ubuntu: `apt install -y libssl-dev`
- CentOS: `yum install -y openssl-devel`

### 3. 服务器启动后立即退出

**原因**：端口被占用或权限问题

**解决**：
- 检查端口占用：`lsof -i :5555`
- 使用 root 权限运行或修改端口

### 4. 客户端连接超时

**原因**：网络不通或服务器地址错误

**解决**：
- 使用 `ping` 测试服务器连通性
- 检查服务器 IP 地址是否正确
- 检查云服务器安全组规则

---

## 📚 相关文档

- [BUILD_AND_TEST.md](BUILD_AND_TEST.md) - 编译与测试指南
- [QUICKSTART.md](QUICKSTART.md) - 快速开始指南
- [TEST_VERIFICATION.md](TEST_VERIFICATION.md) - 测试验证指南

---

## 💡 建议

1. **代码托管**：优先使用 GitHub，便于协作和版本管理
2. **服务器部署**：生产环境推荐使用 Docker + systemd，便于管理和维护
3. **域名配置**：使用域名代替 IP 地址，便于迁移和配置
4. **HTTPS/WSS**：生产环境建议配置 SSL 证书，使用 WSS 协议
5. **监控告警**：配置服务器监控和日志收集，及时发现问题
