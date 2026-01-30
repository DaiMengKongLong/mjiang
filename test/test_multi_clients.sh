#!/bin/bash

# 多客户端测试脚本
# 同时启动 4 个测试客户端，模拟 4 个玩家加入同一房间

echo "=========================================="
echo "多客户端测试脚本"
echo "=========================================="
echo "将启动 4 个测试客户端，模拟 4 个玩家"
echo "确保服务器已启动: ./mahjong_server_ws"
echo "=========================================="
echo ""

# 检查 Node.js 是否安装
if ! command -v node &> /dev/null; then
    echo "❌ 错误: 未找到 Node.js"
    echo "请先安装 Node.js: https://nodejs.org/"
    exit 1
fi

# 检查 test_client.js 是否存在
if [ ! -f "test_client.js" ]; then
    echo "❌ 错误: 未找到 test_client.js"
    echo "请确保在 test/ 目录下运行此脚本"
    exit 1
fi

# 房间ID
ROOM_ID="test_room_$(date +%s)"

echo "房间ID: $ROOM_ID"
echo ""
echo "启动 4 个客户端..."
echo ""

# 启动 4 个客户端（每个在后台运行）
node test_client.js player_001 玩家1 $ROOM_ID &
sleep 0.5

node test_client.js player_002 玩家2 $ROOM_ID &
sleep 0.5

node test_client.js player_003 玩家3 $ROOM_ID &
sleep 0.5

node test_client.js player_004 玩家4 $ROOM_ID &
sleep 0.5

echo ""
echo "✅ 4 个客户端已启动"
echo "查看上面的输出，观察房间信息和游戏流程"
echo ""
echo "按 Ctrl+C 停止所有客户端"
echo ""

# 等待所有后台进程
wait
