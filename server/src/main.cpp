// 最简麻将服务器骨架（带假数据 TCP 通信）
// 当前版本功能：
// 1. 在本机 5555 端口启动阻塞式 TCP 监听；
// 2. 接受单个客户端连接；
// 3. 连接建立后，向客户端发送几条协议文档中定义的 JSON 测试消息（room_info / deal_cards 等）；
// 4. 持续读取客户端发来的文本行，并打印到控制台。
//
// 说明：
// - 为了避免依赖第三方库，这里使用最基础的 BSD socket 接口；
// - JSON 仅作为“文本字符串”发送与打印，本版本不做解析（后续接入 GameEngine 时再补解析逻辑）。

#include <iostream>
#include <string>
#include <vector>
#include <cstring>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

namespace {

const int kServerPort = 5555;

// 向客户端发送一行文本（以 '\n' 结尾）
void sendLine(int clientFd, const std::string& line) {
    std::string data = line;
    data.push_back('\n');
    ssize_t sent = ::send(clientFd, data.c_str(), data.size(), 0);
    if (sent < 0) {
        std::perror("[mahjong_server] send failed");
    }
}

// 从客户端按行读取文本（遇到 '\n' 结束），简单阻塞读取
bool recvLine(int clientFd, std::string& outLine) {
    outLine.clear();
    char ch;
    while (true) {
        ssize_t n = ::recv(clientFd, &ch, 1, 0);
        if (n <= 0) {
            return false; // 连接关闭或出错
        }
        if (ch == '\n') {
            break;
        }
        outLine.push_back(ch);
    }
    return true;
}

} // namespace

int main() {
    std::cout << "[mahjong_server] 启动，监听端口 " << kServerPort << std::endl;

    int listenFd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (listenFd < 0) {
        std::perror("[mahjong_server] 创建 socket 失败");
        return 1;
    }

    int opt = 1;
    ::setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(kServerPort);

    if (::bind(listenFd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        std::perror("[mahjong_server] 绑定端口失败");
        ::close(listenFd);
        return 1;
    }

    if (::listen(listenFd, 1) < 0) {
        std::perror("[mahjong_server] listen 失败");
        ::close(listenFd);
        return 1;
    }

    std::cout << "[mahjong_server] 等待客户端连接..." << std::endl;

    sockaddr_in clientAddr;
    socklen_t clientLen = sizeof(clientAddr);
    int clientFd = ::accept(listenFd, reinterpret_cast<sockaddr*>(&clientAddr), &clientLen);
    if (clientFd < 0) {
        std::perror("[mahjong_server] accept 失败");
        ::close(listenFd);
        return 1;
    }

    std::cout << "[mahjong_server] 客户端已连接，发送测试 JSON 消息。" << std::endl;

    // 发送一组简单的测试 JSON 消息（与 protocol.md 对应）
    sendLine(clientFd, R"({"type":"room_info","roomId":"test_room","players":[{"seat":0,"playerId":"user_001","nickname":"玩家1"}]})");
    sendLine(clientFd, R"({"type":"deal_cards","seat":0,"handCards":[1,2,3,4,5,6,7,8,9,10,11,12,13]})");
    sendLine(clientFd, R"({"type":"your_turn","allowedActions":["PLAY_CARD"]})");

    // 循环读取客户端发来的消息并打印
    while (true) {
        std::string line;
        if (!recvLine(clientFd, line)) {
            std::cout << "[mahjong_server] 客户端断开连接或读取失败，退出。" << std::endl;
            break;
        }
        std::cout << "[mahjong_server] 收到客户端消息: " << line << std::endl;
    }

    ::close(clientFd);
    ::close(listenFd);
    return 0;
}

