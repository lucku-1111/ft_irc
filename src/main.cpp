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

void sendFd(int fd, std::string str) {
    int ret = send(fd, str.c_str(), str.size(), 0);
    if (ret == -1) {
        std::cerr << str.c_str() << "\n";
    }
    std::cout << "========== send client " << fd << " ==========\n";
    std::cout << str << "\n\n";
    return;
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