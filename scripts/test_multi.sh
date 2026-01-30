#!/bin/bash

# 多客户端测试脚本
# 使用方法: ./scripts/test_multi.sh

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

echo "=========================================="
echo "多客户端测试脚本"
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

# 生成房间ID
ROOM_ID="test_room_$(date +%s)"

echo "房间ID: $ROOM_ID"
echo "将启动 4 个测试客户端，模拟 4 个玩家"
echo ""
echo "=========================================="
echo ""

# 启动 4 个客户端（每个在后台运行）
cd "$TEST_DIR"

echo "启动客户端 1..."
node test_client.js player_001 玩家1 "$ROOM_ID" > /tmp/mahjong_client_1.log 2>&1 &
CLIENT1_PID=$!
sleep 0.5

echo "启动客户端 2..."
node test_client.js player_002 玩家2 "$ROOM_ID" > /tmp/mahjong_client_2.log 2>&1 &
CLIENT2_PID=$!
sleep 0.5

echo "启动客户端 3..."
node test_client.js player_003 玩家3 "$ROOM_ID" > /tmp/mahjong_client_3.log 2>&1 &
CLIENT3_PID=$!
sleep 0.5

echo "启动客户端 4..."
node test_client.js player_004 玩家4 "$ROOM_ID" > /tmp/mahjong_client_4.log 2>&1 &
CLIENT4_PID=$!
sleep 0.5

echo ""
echo -e "${GREEN}✅ 4 个客户端已启动${NC}"
echo ""
echo "客户端日志:"
echo "  客户端 1: tail -f /tmp/mahjong_client_1.log"
echo "  客户端 2: tail -f /tmp/mahjong_client_2.log"
echo "  客户端 3: tail -f /tmp/mahjong_client_3.log"
echo "  客户端 4: tail -f /tmp/mahjong_client_4.log"
echo ""
echo "按 Ctrl+C 停止所有客户端"
echo ""

# 等待所有后台进程
trap "kill $CLIENT1_PID $CLIENT2_PID $CLIENT3_PID $CLIENT4_PID 2>/dev/null; exit" INT TERM
wait
