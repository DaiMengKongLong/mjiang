cmake_minimum_required(VERSION 3.10)
project(mahjong_server LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# TCP 版本服务器（原始版本，不使用新的 NetPlayer）
add_executable(mahjong_server
    src/main.cpp
    src/Room.cpp
    # 注意：TCP 版本不使用新的 NetPlayer（依赖 WebSocketServer）
)

target_include_directories(mahjong_server PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# WebSocket 版本服务器
add_executable(mahjong_server_ws
    src/main_websocket.cpp
    src/WebSocketServer.cpp
    src/MessageHandler.cpp
    src/JsonHelper.cpp
    src/Room.cpp
    src/NetPlayer.cpp
    # 游戏逻辑
    src/game/GameEngine.cpp
    src/game/GameLogic.cpp
)

target_include_directories(mahjong_server_ws PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game
)

# 定义 USE_GAME_ENGINE 宏，启用 GameEngine
target_compile_definitions(mahjong_server_ws PRIVATE USE_GAME_ENGINE)

# 链接 OpenSSL（用于 SHA1 和 Base64）
find_package(OpenSSL REQUIRED)
target_link_libraries(mahjong_server_ws PRIVATE OpenSSL::SSL OpenSSL::Crypto)
