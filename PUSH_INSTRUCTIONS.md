# 推送代码到 GitHub 的步骤

## 当前状态
- ✅ Git 仓库已初始化
- ✅ 代码已提交（55 个文件）
- ✅ 远程仓库已配置：`https://github.com/DaiMengKongLong/mjiang.git`
- ⏳ 等待推送代码

## 推送步骤

### 方式 1：使用 Personal Access Token（推荐）

1. **获取 Personal Access Token**：
   - 访问：https://github.com/settings/tokens
   - 点击 "Generate new token (classic)"
   - 填写 Note（如：`mjiang-project`）
   - 勾选 `repo` 权限
   - 点击 "Generate token"
   - **复制 token**（只显示一次，请保存好）

2. **推送代码**：
   ```bash
   cd /Users/ouyjay/Desktop/麻将2026-01-30
   git push -u origin main
   ```

3. **输入凭据**：
   - Username: `DaiMengKongLong`
   - Password: **粘贴你的 Personal Access Token**（不是账户密码）

### 方式 2：在 URL 中包含 Token（一次性）

```bash
cd /Users/ouyjay/Desktop/麻将2026-01-30

# 替换 YOUR_TOKEN 为你的 Personal Access Token
git remote set-url origin https://YOUR_TOKEN@github.com/DaiMengKongLong/mjiang.git

# 推送
git push -u origin main
```

### 方式 3：配置 SSH 密钥（长期方案）

如果你希望使用 SSH 方式，需要为 `DaiMengKongLong` 账户配置 SSH 密钥：

1. **生成新的 SSH 密钥**（如果还没有）：
   ```bash
   ssh-keygen -t ed25519 -C "daimengkonglong@outlook.com" -f ~/.ssh/id_ed25519_daimengkonglong
   ```

2. **添加 SSH 密钥到 ssh-agent**：
   ```bash
   eval "$(ssh-agent -s)"
   ssh-add ~/.ssh/id_ed25519_daimengkonglong
   ```

3. **复制公钥**：
   ```bash
   cat ~/.ssh/id_ed25519_daimengkonglong.pub
   ```

4. **添加到 GitHub**：
   - 访问：https://github.com/settings/keys
   - 点击 "New SSH key"
   - 粘贴公钥内容
   - 保存

5. **配置 Git 使用新密钥**：
   ```bash
   # 创建 SSH 配置文件
   cat >> ~/.ssh/config << EOF
   Host github.com-daimengkonglong
       HostName github.com
       User git
       IdentityFile ~/.ssh/id_ed25519_daimengkonglong
   EOF
   
   # 更新远程仓库地址
   git remote set-url origin git@github.com-daimengkonglong:DaiMengKongLong/mjiang.git
   
   # 推送
   git push -u origin main
   ```

## 快速推送命令

**最简单的方式**（使用 Personal Access Token）：

```bash
cd /Users/ouyjay/Desktop/麻将2026-01-30
git push -u origin main
```

然后输入：
- Username: `DaiMengKongLong`
- Password: `你的 Personal Access Token`

## 验证推送

推送成功后，访问 https://github.com/DaiMengKongLong/mjiang 查看代码。

## 后续更新

推送成功后，后续更新代码只需：

```bash
git add .
git commit -m "更新说明"
git push
```
