#include "../inc/Server.hpp"

bool checkPort(char *str) {
    int port = atoi(str);

    if (port < 1024 || port > 65535)
        return (false);
    return (true);
}

bool checkPwd(std::string pwd) {
    if (pwd.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890_") != std::string::npos)
        return (false);
    return (true);
}

void sendFd(int fd, const std::string &str) {
    size_t totalSent = 0; // 전송된 총 바이트 수
    ssize_t sent; // 한 번의 send 호출로 전송된 바이트 수
    size_t strSize = str.size(); // 전송해야 할 전체 문자열의 크기

    // 문자열의 끝에 도달하거나 send 함수에서 오류가 발생할 때까지 반복
    while (totalSent < strSize) {
        sent = send(fd, str.c_str() + totalSent, strSize - totalSent, 0);
        if (sent == -1) {
            // 오류 처리: EAGAIN 또는 EWOULDBLOCK은 재시도를 의미할 수 있음
            // 블로킹 모드에서는 이러한 오류는 일반적으로 발생하지 않음
            std::cerr << "Failed to send data: " << strerror(errno) << "\n";
            break; // 실패한 경우 반복 중단
        }
        totalSent += sent; // 전송된 바이트 수 업데이트
    }

    if (totalSent == strSize) {
        std::cout << "========== send client " << fd << " ==========\n";
        std::cout << str << "\n\n";
    } else {
        std::cerr << "Failed to send the entire message\n";
    }
}

int main(int ac, char **av) {
    if (ac != 3 || !checkPort(av[1]) || !checkPwd(av[2])) {
        std::cout << "Invalid arguments" << std::endl;
        return (1);
    }

    Server serv(atoi(av[1]), av[2]);

    serv.startServ();

    return (0);
}