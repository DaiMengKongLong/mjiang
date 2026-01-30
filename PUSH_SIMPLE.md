# 一键推送指南

## 🚀 最简单的方法

### 方法 1：通过环境变量（推荐，最安全）

```bash
cd /Users/ouyjay/Desktop/麻将2026-01-30

# 设置 Token（替换 YOUR_TOKEN 为你的实际 token）
export GITHUB_TOKEN=YOUR_TOKEN

# 推送
./scripts/push_with_token.sh
```

### 方法 2：通过命令行参数

```bash
cd /Users/ouyjay/Desktop/麻将2026-01-30

# 直接传递 Token（替换 YOUR_TOKEN）
./scripts/push_with_token.sh YOUR_TOKEN
```

### 方法 3：交互式输入

```bash
cd /Users/ouyjay/Desktop/麻将2026-01-30

# 运行脚本，会提示输入 Token
./scripts/push_with_token.sh
```

---

## 📝 获取 Personal Access Token

1. 访问：https://github.com/settings/tokens
2. 点击 "Generate new token (classic)"
3. 填写 Note：`mjiang-project`
4. **勾选 `repo` 权限**（重要！）
5. 点击 "Generate token"
6. **复制 token**（只显示一次）

---

## ✅ 完整示例

```bash
# 1. 进入项目目录
cd /Users/ouyjay/Desktop/麻将2026-01-30

# 2. 设置 Token（替换为你的实际 token）
export GITHUB_TOKEN=ghp_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

# 3. 推送
./scripts/push_with_token.sh
```

---

## 🔒 安全提示

- Token 会临时保存在 URL 中用于推送
- 推送完成后会自动清除 URL 中的 Token
- 建议使用环境变量方式，避免在命令行历史中留下 Token
- 不要将 Token 提交到代码仓库

---

## 📍 推送成功后

访问：https://github.com/DaiMengKongLong/mjiang 查看你的代码
