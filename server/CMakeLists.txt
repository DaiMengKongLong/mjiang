cmake_minimum_required(VERSION 3.10)
project(mahjong_server LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# TCP 版本服务器（原始版本）
add_executable(mahjong_server
    src/main.cpp
    src/Room.cpp
    src/NetPlayer.cpp
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
)

target_include_directories(mahjong_server_ws PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# 链接 OpenSSL（用于 SHA1 和 Base64）
find_package(OpenSSL REQUIRED)
target_link_libraries(mahjong_server_ws PRIVATE OpenSSL::SSL OpenSSL::Crypto)
