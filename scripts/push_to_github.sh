#!/bin/bash

# 推送代码到 GitHub 的交互式脚本

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"

cd "$PROJECT_DIR"

echo "=========================================="
echo "  推送代码到 GitHub"
echo "=========================================="
echo ""
echo "仓库地址: https://github.com/DaiMengKongLong/mjiang.git"
echo ""

# 检查远程仓库
if ! git remote get-url origin > /dev/null 2>&1; then
    echo "⚠️  未配置远程仓库，正在配置..."
    git remote add origin https://github.com/DaiMengKongLong/mjiang.git
fi

# 检查是否有未提交的更改
if [ -n "$(git status --porcelain)" ]; then
    echo "📝 检测到未提交的更改："
    git status --short
    echo ""
    read -p "是否先提交这些更改？(y/n) " -n 1 -r
    echo ""
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        read -p "请输入提交信息: " commit_msg
        if [ -z "$commit_msg" ]; then
            commit_msg="更新代码"
        fi
        git add .
        git commit -m "$commit_msg"
        echo "✅ 更改已提交"
        echo ""
    fi
fi

# 清除可能存在的旧凭据
echo "🔐 清除旧的 GitHub 凭据..."
git credential-osxkeychain erase <<EOF
host=github.com
protocol=https
EOF
echo "✅ 凭据已清除"
echo ""

# 设置远程仓库 URL（包含用户名）
git remote set-url origin https://DaiMengKongLong@github.com/DaiMengKongLong/mjiang.git

echo "🚀 开始推送代码到 GitHub..."
echo ""
echo "⚠️  如果提示输入密码，请使用 Personal Access Token（不是账户密码）"
echo "   获取 Token: https://github.com/settings/tokens"
echo ""

# 尝试推送
if git push -u origin main; then
    echo ""
    echo "=========================================="
    echo "  ✅ 推送成功！"
    echo "=========================================="
    echo ""
    echo "查看代码: https://github.com/DaiMengKongLong/mjiang"
    echo ""
else
    echo ""
    echo "=========================================="
    echo "  ❌ 推送失败"
    echo "=========================================="
    echo ""
    echo "可能的原因："
    echo "1. 需要输入 Personal Access Token"
    echo "2. Token 权限不足（需要 repo 权限）"
    echo ""
    echo "解决方案："
    echo "1. 访问 https://github.com/settings/tokens"
    echo "2. 生成新的 token（勾选 repo 权限）"
    echo "3. 重新运行此脚本，在提示输入密码时粘贴 token"
    echo ""
    exit 1
fi
