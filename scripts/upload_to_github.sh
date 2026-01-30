#!/bin/bash

# 快速上传到 GitHub 脚本
# 使用方法: ./scripts/upload_to_github.sh

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"

cd "$PROJECT_DIR"

echo "=========================================="
echo "  麻将联机项目 - GitHub 上传助手"
echo "=========================================="
echo ""

# 检查是否已初始化 Git
if [ ! -d ".git" ]; then
    echo "⚠️  未检测到 Git 仓库，正在初始化..."
    git init
    echo "✅ Git 仓库初始化完成"
    echo ""
fi

# 检查是否有未提交的更改
if [ -z "$(git status --porcelain)" ]; then
    echo "✅ 没有需要提交的更改"
    echo ""
else
    echo "📝 检测到以下更改："
    git status --short
    echo ""
    
    read -p "是否提交这些更改？(y/n) " -n 1 -r
    echo ""
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        read -p "请输入提交信息（直接回车使用默认）: " commit_msg
        if [ -z "$commit_msg" ]; then
            commit_msg="更新项目文件"
        fi
        
        git add .
        git commit -m "$commit_msg"
        echo "✅ 更改已提交"
        echo ""
    fi
fi

# 检查是否已设置远程仓库
if git remote | grep -q "^origin$"; then
    REMOTE_URL=$(git remote get-url origin)
    echo "📍 当前远程仓库: $REMOTE_URL"
    echo ""
    
    read -p "是否推送到 GitHub？(y/n) " -n 1 -r
    echo ""
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        BRANCH=$(git branch --show-current)
        if [ -z "$BRANCH" ]; then
            BRANCH="main"
            git branch -M main
        fi
        
        echo "🚀 正在推送到 GitHub..."
        git push -u origin "$BRANCH"
        echo "✅ 推送完成！"
    fi
else
    echo "⚠️  未设置远程仓库"
    echo ""
    echo "请先执行以下命令设置远程仓库："
    echo ""
    echo "  # HTTPS 方式"
    echo "  git remote add origin https://github.com/YOUR_USERNAME/YOUR_REPO_NAME.git"
    echo ""
    echo "  # SSH 方式（推荐）"
    echo "  git remote add origin git@github.com:YOUR_USERNAME/YOUR_REPO_NAME.git"
    echo ""
    echo "然后重新运行此脚本"
    echo ""
    echo "📖 详细步骤请查看: DEPLOYMENT_GUIDE.md"
fi

echo ""
echo "=========================================="
echo "  完成！"
echo "=========================================="
