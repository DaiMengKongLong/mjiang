#!/bin/bash

# 测试客户端启动脚本
# 使用方法: ./scripts/test_client.sh [playerId] [nickname] [roomId]

set -e

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# 获取脚本所在目录的绝对路径
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
TEST_DIR="$PROJECT_ROOT/test"

# 默认参数
PLAYER_ID=${1:-"player_$(date +%s)"}
NICKNAME=${2:-"测试玩家$(date +%s | tail -c 4)"}
ROOM_ID=${3:-"test_room_$(date +%s)"}

echo "=========================================="
echo "测试客户端启动脚本"
echo "=========================================="
echo ""

# 检查 Node.js 是否安装
if ! command -v node &> /dev/null; then
    echo -e "${RED}❌ 错误: 未找到 Node.js${NC}"
    echo "请先安装 Node.js: https://nodejs.org/"
    exit 1
fi

# 检查测试客户端文件是否存在
if [ ! -f "$TEST_DIR/test_client.js" ]; then
    echo -e "${RED}❌ 错误: 未找到 test_client.js${NC}"
    echo "请确保测试文件存在于: $TEST_DIR/test_client.js"
    exit 1
fi

# 检查 ws 模块是否安装
if [ ! -d "$TEST_DIR/node_modules/ws" ]; then
    echo -e "${YELLOW}⚠️  ws 模块未安装，正在安装...${NC}"
    cd "$TEST_DIR"
    npm install ws || {
        echo -e "${RED}❌ 安装失败${NC}"
        exit 1
    }
    echo -e "${GREEN}✅ 安装成功${NC}"
    echo ""
fi

# 检查服务器是否运行
if ! lsof -Pi :5555 -sTCP:LISTEN -t >/dev/null 2>&1 ; then
    echo -e "${YELLOW}⚠️  警告: 服务器似乎未运行${NC}"
    echo "请先启动服务器: ./scripts/start_server.sh"
    echo ""
    read -p "是否继续？(y/n) " -n 1 -r
    echo ""
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        exit 1
    fi
fi

# 显示连接信息
echo "连接信息:"
echo "  玩家ID: $PLAYER_ID"
echo "  昵称: $NICKNAME"
echo "  房间ID: $ROOM_ID"
echo "  服务器: ws://127.0.0.1:5555"
echo ""
echo "=========================================="
echo ""

# 运行测试客户端
cd "$TEST_DIR"
node test_client.js "$PLAYER_ID" "$NICKNAME" "$ROOM_ID"
