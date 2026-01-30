# 快速上传到云端

## 🚀 3 步上传到 GitHub

### 步骤 1：在 GitHub 创建仓库

1. 访问 https://github.com/new
2. 填写仓库名称（如：`mahjong-online`）
3. 选择 **Public** 或 **Private**
4. **不要**勾选 "Initialize this repository with a README"
5. 点击 "Create repository"

### 步骤 2：连接本地仓库

```bash
cd /Users/ouyjay/Desktop/麻将2026-01-30

# 方式 1：使用 HTTPS（需要 Personal Access Token）
git remote add origin https://github.com/YOUR_USERNAME/YOUR_REPO_NAME.git

# 方式 2：使用 SSH（推荐，需要配置 SSH 密钥）
git remote add origin git@github.com:YOUR_USERNAME/YOUR_REPO_NAME.git
```

**替换 `YOUR_USERNAME` 和 `YOUR_REPO_NAME` 为你的实际值**

### 步骤 3：提交并推送

```bash
# 提交所有文件
git add .
git commit -m "初始提交：麻将联机项目基础架构"

# 推送到 GitHub
git branch -M main
git push -u origin main
```

**如果使用 HTTPS，GitHub 会要求输入：**
- 用户名：你的 GitHub 用户名
- 密码：使用 **Personal Access Token**（不是账户密码）

**如何获取 Personal Access Token：**
1. GitHub → Settings → Developer settings → Personal access tokens → Tokens (classic)
2. Generate new token (classic)
3. 勾选 `repo` 权限
4. 生成后复制 token（只显示一次）

---

## 🎯 使用便捷脚本（推荐）

```bash
# 运行上传脚本
./scripts/upload_to_github.sh
```

脚本会自动：
- 检查 Git 仓库状态
- 提示提交更改
- 推送到 GitHub（如果已设置远程仓库）

---

## 📦 后续更新代码

```bash
# 添加修改
git add .

# 提交更改
git commit -m "描述你的更改"

# 推送到 GitHub
git push
```

或使用便捷脚本：
```bash
./scripts/upload_to_github.sh
```

---

## 🌐 部署到云服务器

详细步骤请查看：[DEPLOYMENT_GUIDE.md](DEPLOYMENT_GUIDE.md)

**快速部署命令：**

```bash
# 1. 在云服务器上克隆仓库
ssh root@YOUR_SERVER_IP
cd /opt
git clone https://github.com/YOUR_USERNAME/YOUR_REPO_NAME.git
cd YOUR_REPO_NAME

# 2. 编译服务器
cd server
mkdir build && cd build
cmake ..
make

# 3. 运行服务器
./mahjong_server_ws
```

---

## ❓ 常见问题

### Q: 提示 "remote origin already exists"
**A:** 已存在远程仓库，可以：
- 查看：`git remote -v`
- 删除：`git remote remove origin`，然后重新添加
- 或直接推送：`git push -u origin main`

### Q: 提示 "Permission denied"
**A:** 
- HTTPS：检查 Personal Access Token 是否正确
- SSH：需要配置 SSH 密钥（参考 GitHub 文档）

### Q: 如何配置 SSH 密钥？
**A:** 
```bash
# 1. 生成 SSH 密钥
ssh-keygen -t ed25519 -C "your_email@example.com"

# 2. 复制公钥
cat ~/.ssh/id_ed25519.pub

# 3. 在 GitHub → Settings → SSH and GPG keys → New SSH key
#    粘贴公钥内容
```

---

## 📚 更多信息

- **完整部署指南**：[DEPLOYMENT_GUIDE.md](DEPLOYMENT_GUIDE.md)
- **项目文档**：[DOCS_INDEX.md](DOCS_INDEX.md)
