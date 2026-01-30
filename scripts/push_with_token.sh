#!/bin/bash

# 使用 Token 推送代码到 GitHub
# 使用方法：
#   1. 通过环境变量: GITHUB_TOKEN=your_token ./scripts/push_with_token.sh
#   2. 通过参数: ./scripts/push_with_token.sh your_token
#   3. 交互式输入: ./scripts/push_with_token.sh

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"

cd "$PROJECT_DIR"

# 获取 Token
if [ -n "$1" ]; then
    # 从命令行参数获取
    GITHUB_TOKEN="$1"
elif [ -n "$GITHUB_TOKEN" ]; then
    # 从环境变量获取
    echo "✅ 使用环境变量中的 GITHUB_TOKEN"
else
    # 交互式输入
    echo "=========================================="
    echo "  推送代码到 GitHub"
    echo "=========================================="
    echo ""
    echo "请提供您的 Personal Access Token"
    echo "获取 Token: https://github.com/settings/tokens"
    echo ""
    read -sp "请输入 Token: " GITHUB_TOKEN
    echo ""
    echo ""
fi

if [ -z "$GITHUB_TOKEN" ]; then
    echo "❌ 错误：未提供 Token"
    exit 1
fi

# 设置远程仓库 URL（包含 token）
echo "🔐 配置远程仓库..."
git remote set-url origin https://${GITHUB_TOKEN}@github.com/DaiMengKongLong/mjiang.git

# 推送
echo "🚀 开始推送代码..."
if git push -u origin main; then
    echo ""
    echo "=========================================="
    echo "  ✅ 推送成功！"
    echo "=========================================="
    echo ""
    echo "查看代码: https://github.com/DaiMengKongLong/mjiang"
    echo ""
    
    # 恢复远程仓库 URL（不包含 token，更安全）
    git remote set-url origin https://github.com/DaiMengKongLong/mjiang.git
    echo "✅ 已清除 URL 中的 Token（安全措施）"
else
    echo ""
    echo "=========================================="
    echo "  ❌ 推送失败"
    echo "=========================================="
    echo ""
    echo "可能的原因："
    echo "1. Token 无效或已过期"
    echo "2. Token 权限不足（需要 repo 权限）"
    echo "3. 网络连接问题"
    echo ""
    
    # 恢复远程仓库 URL
    git remote set-url origin https://github.com/DaiMengKongLong/mjiang.git
    exit 1
fi
