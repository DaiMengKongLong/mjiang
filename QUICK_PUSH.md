# 快速推送指南

## 最简单的方法

### 步骤 1：获取 Personal Access Token

1. 访问：https://github.com/settings/tokens
2. 点击 "Generate new token (classic)"
3. 填写 Note：`mjiang-project`
4. **勾选 `repo` 权限**（重要！）
5. 点击 "Generate token"
6. **复制 token**（只显示一次，请立即复制）

### 步骤 2：在终端执行推送

打开终端，执行：

```bash
cd /Users/ouyjay/Desktop/麻将2026-01-30

# 方法 A：使用脚本（推荐）
./scripts/push_to_github.sh
# 当提示输入密码时，粘贴你的 token

# 方法 B：直接推送
git push -u origin main
# Username: DaiMengKongLong
# Password: 粘贴你的 token
```

### 步骤 3：验证

推送成功后，访问：https://github.com/DaiMengKongLong/mjiang

---

## 如果不想每次输入 Token

可以在 URL 中包含 token（注意：token 会保存在 Git 配置中）：

```bash
cd /Users/ouyjay/Desktop/麻将2026-01-30

# 替换 YOUR_TOKEN 为你的实际 token
git remote set-url origin https://YOUR_TOKEN@github.com/DaiMengKongLong/mjiang.git

# 推送
git push -u origin main
```

**注意**：这种方式 token 会保存在 Git 配置中，建议只在个人开发机器上使用。

---

## 当前状态

- ✅ 代码已提交（57 个文件）
- ✅ 远程仓库已配置
- ⏳ 等待推送（需要 Personal Access Token）
