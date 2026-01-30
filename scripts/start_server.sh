#!/bin/bash

# 麻将服务器启动脚本
# 使用方法: ./scripts/start_server.sh

set -e

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# 获取脚本所在目录的绝对路径
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
SERVER_BUILD_DIR="$PROJECT_ROOT/server/build"

echo "=========================================="
echo "麻将服务器启动脚本"
echo "=========================================="
echo ""

# 检查服务器可执行文件是否存在
if [ ! -f "$SERVER_BUILD_DIR/mahjong_server_ws" ]; then
    echo -e "${YELLOW}⚠️  服务器可执行文件不存在，开始编译...${NC}"
    echo ""
    
    # 检查 build 目录是否存在
    if [ ! -d "$SERVER_BUILD_DIR" ]; then
        echo "创建 build 目录..."
        mkdir -p "$SERVER_BUILD_DIR"
    fi
    
    # 编译服务器
    cd "$SERVER_BUILD_DIR"
    echo "运行 CMake..."
    cmake .. || {
        echo -e "${RED}❌ CMake 配置失败${NC}"
        exit 1
    }
    
    echo "编译服务器..."
    make || {
        echo -e "${RED}❌ 编译失败${NC}"
        exit 1
    }
    
    echo -e "${GREEN}✅ 编译成功！${NC}"
    echo ""
fi

# 检查端口是否被占用
if lsof -Pi :5555 -sTCP:LISTEN -t >/dev/null 2>&1 ; then
    echo -e "${YELLOW}⚠️  端口 5555 已被占用${NC}"
    echo "正在运行的进程："
    lsof -Pi :5555 -sTCP:LISTEN
    echo ""
    read -p "是否要终止占用端口的进程？(y/n) " -n 1 -r
    echo ""
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        echo "终止进程..."
        lsof -ti :5555 | xargs kill -9 2>/dev/null || true
        sleep 1
    else
        echo "退出"
        exit 1
    fi
fi

# 启动服务器
echo -e "${GREEN}🚀 启动服务器...${NC}"
echo "服务器地址: ws://127.0.0.1:5555"
echo "按 Ctrl+C 停止服务器"
echo "=========================================="
echo ""

cd "$SERVER_BUILD_DIR"
./mahjong_server_ws
