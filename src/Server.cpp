#include "../inc/Server.hpp"
#include "../inc/msgs.hpp"

void Server::sendAll(int fd, std::string msg) {
    std::map<std::string, Channel *> chs = _clients[fd].getClientChannels();

    for (std::map<std::string, Channel *>::iterator it = chs.begin(); it != chs.end(); it++) {
        it->second->sendToAllClients(fd, msg);
    }
}

void send_fd(int fd, std::string str) {
    int ret = send(fd, str.c_str(), str.size(), 0);
    if (ret == -1) {
        std::cerr << str.c_str() << "\n";
    }
    std::cout << "========== send client " << fd << " ==========\n";
    std::cout << str << "\n\n";
    return;
}


Server::Server(int port, std::string pwd) : _pwd(pwd) {
    try {
        if ((_servFd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
            throw std::runtime_error("server socket does not generate");

        _servAddr.sin_family = AF_INET;
        _servAddr.sin_addr.s_addr = INADDR_ANY;
        _servAddr.sin_port = htons(port);

        if (bind(_servFd, (struct sockaddr *) &_servAddr, sizeof(_servAddr)) == -1)
            throw std::runtime_error("bind function fail");

        if (listen(_servFd, MAX_CLIENT) == -1)
            throw std::runtime_error("listen function fail");
    } catch (const std::runtime_error &e) {
        if (_servFd != -1)
            close(_servFd);
        std::cerr << "Error: " << e.what() << std::endl;
        exit(1);
    }

    _pollFds.push_back((struct pollfd) {_servFd, POLLIN, 0});
}

void Server::acceptClient() {
    if (_pollFds.size() > 101) {
        std::cerr << "Error: connection limit exceeded" << std::endl;
        return;
    }

    struct sockaddr_in clientAddr;
    socklen_t clientLen = sizeof(clientAddr);
    int clientFd = accept(_servFd, (struct sockaddr *) &clientAddr, &clientLen);
    if (clientFd == -1) {
        std::cerr << "Error: client socket does not generate" << std::endl;
        return;
    }
    _pollFds.push_back((struct pollfd) {clientFd, POLLIN | POLLHUP, 0});
    _pollFds[0].revents = 0;
    fcntl(clientFd, F_SETFL, O_NONBLOCK);

    _clients[clientFd] = Client(clientFd);
}

void Server::recvClient(int i) {
    int count = 0;
    int clientFd = _pollFds[i].fd;
    char buf[BUF_SIZE] = {0,};

    count = recv(clientFd, buf, BUF_SIZE, 0);

    if (count <= 0) {
        if (count == 0) {
            std::cerr << "Client(fd: " << clientFd << ") is disconnected" << std::endl;
            cmdQuit(clientFd, i);
            return;
        }
        std::cerr << "Error: recv function fail" << std::endl;
        _lines[i].clear();
        close(clientFd);
        _pollFds.erase(_pollFds.begin() + i);
        for (std::map<std::string, Channel>::iterator it = _channels.begin(); it != _channels.end(); it++) {
            it->second.removeClient(clientFd);
        }
        _clients.erase(clientFd);
        return;
    }

    _lines[i] += buf;
    std::string::size_type pos;

    while ((pos = _lines[i].find("\r\n")) != std::string::npos) {
        std::string line = _lines[i].substr(0, pos);
        std::vector<std::string> cmds = splitMsg(line);

        std::cout << "---- command ----\n" << line << std::endl;

        for (int j = 0; j < (int) (cmds.size()); j++)
            std::cout << "msg[" << j << "]: " << cmds[j] << std::endl;
        executeCommand(clientFd, cmds, i);
        _lines[i].erase(0, pos + 2);
    }
}

void Server::startServ() {
    std::cout << "server opened" << std::endl;

    while (poll(&_pollFds[0], _pollFds.size(), -1)) {
        if (_pollFds[0].revents & POLLIN)
            this->acceptClient();
        for (int i = 1; i < (int) (_pollFds.size()); i++) {
            if (_pollFds[i].revents & (POLLIN | POLLHUP)) {
                std::cout << "i: " << i << std::endl;
                this->recvClient(i);
            }
        }
    }
}

std::vector<std::string> splitMsg(std::string line) {
    std::vector<std::string> command;
    std::string::size_type start = 0;
    std::string::size_type pos;
    bool colon = false;

    while ((pos = line.find(" ", start)) != std::string::npos) {
        if (start != 0 && line[start] == ':') {
            command.push_back(line.substr(start));
            colon = true;
            break;
        }
        command.push_back(line.substr(start, pos - start));
        start = pos + 1;
    }
    if (!colon)
        command.push_back(line.substr(start));

    for (std::vector<std::string>::iterator it = command.begin(); ++it != command.end();) {
        if ((*it)[0] == ':') {
            (*it).erase(0, 1);
            break;
        }
    }

    return (command);
}


// ToDo : 클라이언트 생성은 poll에서 새로운 클라이언트가 들어왔을 때만 생성?
// ToDo : 클라이언트 소켓을 닫을 때 클라이언트 객체도 삭제해야하는지?

///// Command Execution /////

void Server::executeCommand(int fd, std::vector<std::string> cmds, int idx) {

    std::cout << "executeCommand : " << cmds[0] << std::endl;
    std::cout << "fd : " << fd << std::endl;
    std::cout << "status : " << _clients[fd].getStatus() << std::endl;

    // 명령어 처리
    // 명령어 벡터 string으로 받아오기
    std::string cmdType = cmds[0];

    // 클라이언트의 상태에 따라 명령을 처리
    if (_clients[fd].getStatus() == NoPassword) {
        // 비밀번호가 없는 상태에서 pass 명령어인 경우
        if (cmdType == "pass" || cmdType == "PASS")
            // pass 명령어 처리
            cmdPass(fd, cmds);
        else
            // 비밀번호가 없는 상태에서 pass 명령어가 아닌 경우
            send(fd, "CheckIn cmdType must be in order : |PASS| -> |NICK| -> |USER| \r\n", 61, 0);
    } else if (_clients[fd].getStatus() == NoNickname) {
        // 비밀번호는 있고 닉네임이 없는 상태에서 pass, nick 명령어인 경우
        if (cmdType == "pass" || cmdType == "PASS")
            // pass 명령어 처리 : pass는 마지막에 들어온 pass 기준으로 저장
            cmdPass(fd, cmds);
        else if (cmdType == "nick" || cmdType == "NICK")
            // nick 명령어 처리
            cmdNick(fd, cmds);
        else
            // 비밀번호는 있고 닉네임이 없는 상태에서 pass, nick 명령어가 아닌 경우
            send(fd, "CheckIn cmdType must be in order : |PASS| -> |NICK| -> |USER| \r\n", 61, 0);
    } else if (_clients[fd].getStatus() == NoUsername) {
        // 유저네임이 없는 상태에서 pass, nick, user 명령어인 경우
        if (cmdType == "pass" || cmdType == "PASS")
            cmdPass(fd, cmds);
        else if (cmdType == "nick" || cmdType == "NICK")
            cmdNick(fd, cmds);
        else if (cmdType == "user" || cmdType == "USER") {
            cmdUser(fd, cmds);
            // welcome 메시지 전송
            send_fd(fd, RPL_001_WELCOME(_clients[fd].getNickName()));
            send_fd(fd, RPL_002_YOURHOST(_clients[fd].getNickName()));
            send_fd(fd, RPL_003_CREATED(_clients[fd].getNickName()));
            send_fd(fd, RPL_004_SERVERINFO(_clients[fd].getNickName()));

            std::cout << "welcome message sent" << std::endl;
        }
    } else if (_clients[fd].getStatus() == LoggedIn) {
        // 로그인 상태에서 명령어 처리
        if (cmdType == "pass" || cmdType == "PASS")
            send_fd(fd, RPL_462_ALREADYREGISTRED(_clients[fd].getNickName(), "PASS"));
        else if (cmdType == "nick" || cmdType == "NICK")
            cmdNick(fd, cmds);
        else if (cmdType == "user" || cmdType == "USER")
            send_fd(fd, RPL_462_ALREADYREGISTRED(_clients[fd].getNickName(), "USER"));
        else if (cmdType == "join" || cmdType == "JOIN")
            cmdJoin(fd, cmds);
        else if (cmdType == "part" || cmdType == "PART")
            cmdPart(fd, cmds);
        else if (cmdType == "privmsg" || cmdType == "PRIVMSG")
            cmdPrivMsg(fd, cmds);
        else if (cmdType == "kick" || cmdType == "KICK")
            cmdKick(fd, cmds);
        else if (cmdType == "invite" || cmdType == "INVITE")
            cmdInvite(fd, cmds);
        else if (cmdType == "topic" || cmdType == "TOPIC")
            cmdTopic(fd, cmds);
        else if (cmdType == "mode" || cmdType == "MODE")
            cmdMode(fd, cmds);
        else if (cmdType == "quit" || cmdType == "QUIT")
            cmdQuit(fd, idx);
        else if (cmdType == "ping" || cmdType == "PING")
            cmdPing(fd, cmds);
    }

}


///// Command Functions /////
void Server::cmdPass(int fd, std::vector<std::string> cmds) {
    if (cmds.size() < 2)
        send_fd(fd, RPL_461_NEEDMOREPARAMS(_clients[fd].getNickName(), "PASS"));
    else {
        _clients[fd].setPassword(cmds[1]);
        _clients[fd].setIsPasswordSet(true);
    }
}

bool validateNick(std::string nick, std::map<int, Client> clients) {
    if (nick[0] == '#' || nick[0] == ':')
        return (false);

    for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); it++) {
        if (nick == it->second.getNickName())
            return (false);
    }

    return (true);
}

void Server::cmdNick(int fd, std::vector<std::string> cmds) {
    if (cmds.size() < 2 && validateNick(cmds[1], _clients))
        send_fd(fd, RPL_461_NEEDMOREPARAMS(_clients[fd].getNickName(), "NICK"));
    else {
        _clients[fd].setNickName(cmds[1]);
        _clients[fd].setIsNickSet(true);
    }
}

void Server::cmdUser(int fd, std::vector<std::string> cmds) {
    if (cmds.size() < 5)
        send_fd(fd, RPL_461_NEEDMOREPARAMS(_clients[fd].getNickName(), "USER"));
    else {
        _clients[fd].setUserName(cmds[1]);
        _clients[fd].setHostName(cmds[2]);
        _clients[fd].setServerName(cmds[3]);
        _clients[fd].setRealName(cmds[4]);
        _clients[fd].setIsUserSet(true);
    }
}

void Server::cmdJoin(int fd, std::vector<std::string> cmds) {
    if (cmds.size() < 2)
        send_fd(fd, RPL_461_NEEDMOREPARAMS(_clients[fd].getNickName(), "JOIN"));
    else {
        // 채널이름이 #으로 시작하지 않는 경우
        if (cmds[1][0] != '#')
            send_fd(fd, RPL_403_NOSUCHCHANNEL(_clients[fd].getNickName(), cmds[1]));
        else {
            // 채널이름이 #으로 시작하는 경우
            // 채널이름이 이미 존재하는 경우
            if (_channels.find(cmds[1]) != _channels.end()) {

                // 채널에 클라이언트 추가
                _channels[cmds[1]].addClient(fd, &_clients[fd]);

                // 클라이언트에 채널 추가
                _clients[fd].addChannel(cmds[1], &_channels[cmds[1]]);
            } else {

                // 채널이름이 존재하지 않는 경우
                // 채널 생성
                Channel channel(cmds[1]);

                // 서버 채널리스트에 채널 추가
                _channels[cmds[1]] = channel;

                // 채널에 클라이언트 추가
                _channels[cmds[1]].addClient(fd, &_clients[fd]);

                // 클라이언트에 채널 추가
                _clients[fd].addChannel(cmds[1], &_channels[cmds[1]]);

            }
            // 채널에 메시지 전송
            _channels[cmds[1]].sendToAllClients(fd, RPL_JOIN(_clients[fd].getNickName(), _clients[fd].getHostName(), _clients[fd].getServerName(), cmds[1]));
        }
    }
}

void Server::cmdPart(int fd, std::vector<std::string> cmds) {
    if (cmds.size() < 2)
        send_fd(fd, RPL_461_NEEDMOREPARAMS(_clients[fd].getNickName(), "PART"));
    else {
        // 채널이름이 #으로 시작하지 않는 경우
        if (cmds[1][0] != '#')
            send_fd(fd, RPL_403_NOSUCHCHANNEL(_clients[fd].getNickName(), cmds[1]));
        else {
            // 채널이름이 #으로 시작하는 경우
            // 채널이름이 존재하는 경우
            if (_channels.find(cmds[1]) != _channels.end()) {

                // 채널에서 클라이언트 제거
                _channels[cmds[1]].removeClient(fd);

                // 클라이언트에서 채널 제거
                _clients[fd].removeChannel(cmds[1]);

                // 채널에 메시지 전송
                _channels[cmds[1]].sendToAllClients(fd,
                                                    RPL_PART(_clients[fd].getNickName(), _clients[fd].getHostName(), _clients[fd].getServerName(), cmds[1]));
            } else {

                // 채널이름이 존재하지 않는 경우
                send_fd(fd, RPL_403_NOSUCHCHANNEL(_clients[fd].getNickName(), cmds[1]));
            }
        }
    }
}

void Server::cmdPrivMsg(int fd, std::vector<std::string> cmds) {
    if (cmds.size() < 3)
        send_fd(fd, RPL_461_NEEDMOREPARAMS(_clients[fd].getNickName(), "PRIVMSG"));
    else {
        // 수신자가 채널인 경우
        if (cmds[1][0] == '#') {
            // 채널이 존재하는 경우
            if (_channels.find(cmds[1]) != _channels.end()) {
                // 채널에 메시지 전송
                _channels[cmds[1]].sendToAllClients(fd,
                                                    RPL_PRIVMSG(_clients[fd].getNickName(), _clients[fd].getUserName(), _clients[fd].getServerName(), cmds[1],
                                                                cmds[2]));
            } else {
                // 채널이 존재하지 않는 경우
                send_fd(fd, RPL_403_NOSUCHCHANNEL(_clients[fd].getNickName(), cmds[1]));
            }
        } else {
            // 수신자가 클라이언트인 경우
            // 수신자가 존재하는 경우
            std::map<int, Client>::iterator it;

            for (it = _clients.begin(); it != _clients.end(); ++it) {
                if (it->second.getNickName() == cmds[1]) {
                    // 수신자에게 메시지 전송
                    send_fd(it->first, RPL_PRIVMSG(_clients[fd].getNickName(), _clients[fd].getUserName(), _clients[fd].getServerName(), cmds[1], cmds[2]));
                    break;
                }
            }

            if (it == _clients.end()) {
                // 수신자가 존재하지 않는 경우
                send(fd, "401 PRIVMSG :No such nick/channel\r\n", 35, 0);
            }

        }
    }
}

void Server::cmdMode(int fd, std::vector<std::string> cmds) {
    if (cmds.size() < 3)
        send_fd(fd, RPL_461_NEEDMOREPARAMS(_clients[fd].getNickName(), "MODE"));
    else {
        // 채널이름이 #으로 시작하지 않는 경우
        if (cmds[1][0] != '#') {

        } else {
            // 채널이름이 #으로 시작하는 경우
            // 채널이 존재하는 경우
            if (_channels.find(cmds[1]) != _channels.end()) {
                if (cmds[2] == "+i") {
                    // 채널에 초대전용 설정
                    // 유저가 op인지 확인
                    if (_channels[cmds[1]].isClientOP(fd)) {
                        _channels[cmds[1]].setIsInviteOnly(true);
                        send(fd, "MODE :+i\r\n", 9, 0);
                    } else {
                        send(fd, "482 MODE :You're not channel operator\r\n", 38, 0);
                    }
                } else if (cmds[2] == "-i") {
                    // 채널에 초대전용 해제
                    // 유저가 op인지 확인
                    if (_channels[cmds[1]].isClientOP(fd)) {
                        _channels[cmds[1]].setIsInviteOnly(false);
                        send(fd, "MODE :-i\r\n", 10, 0);
                    } else {
                        send(fd, "482 MODE :You're not channel operator\r\n", 38, 0);
                    }
                } else if (cmds[2] == "+o") {
                    if (cmds.size() < 4) {
                        // 닉네임이 들어왔는지 확인
                        send_fd(fd, RPL_461_NEEDMOREPARAMS(_clients[fd].getNickName(), "MODE"));
                    } else if (_channels[cmds[1]].isClientOP(fd)) {
                        // 유저가 op인지 확인
                        // 유저에게 op 권한 부여
                        _channels[cmds[1]].addClientToOPList(fd);
                        send(fd, "MODE :+o\r\n", 9, 0);
                    } else {
                        send(fd, "482 MODE :You're not channel operator\r\n", 38, 0);
                    }
                } else if (cmds[2] == "-o") {
                    if (cmds.size() < 4) {
                        // 닉네임이 들어왔는지 확인
                        send_fd(fd, RPL_461_NEEDMOREPARAMS(_clients[fd].getNickName(), "MODE"));
                    } else if (_channels[cmds[1]].isClientOP(fd)) {
                        // 유저에게 op 권한 해제
                        // 유저가 op인지 확인
                        _channels[cmds[1]].removeClientFromOPList(fd);
                        send(fd, "MODE :-o\r\n", 10, 0);
                    } else {
                        send(fd, "482 MODE :You're not channel operator\r\n", 38, 0);
                    }
                } else if (cmds[2] == "+t") {
                    // op만 토픽 설정 가능
                    // 유저가 op인지 확인
                    if (_channels[cmds[1]].isClientOP(fd)) {
                        _channels[cmds[1]].setIsTopicProtected(true);
                        send(fd, "MODE :+t\r\n", 9, 0);
                    } else {
                        send(fd, "482 MODE :You're not channel operator\r\n", 38, 0);
                    }
                } else if (cmds[2] == "-t") {
                    // 채널에 토픽 해제
                    // 유저가 op인지 확인
                    if (_channels[cmds[1]].isClientOP(fd)) {
                        _channels[cmds[1]].setIsTopicProtected(false);
                        send(fd, "MODE :-t\r\n", 10, 0);
                    } else {
                        send(fd, "482 MODE :You're not channel operator\r\n", 38, 0);
                    }
                } else if (cmds[2] == "+l") {
                    // 채널에 인원제한 설정
                    // 유저가 op인지 확인
                    if (_channels[cmds[1]].isClientOP(fd)) {
                        _channels[cmds[1]].setIsUserLimitSet(true);
                        send(fd, "MODE :+l\r\n", 9, 0);
                    } else {
                        send(fd, "482 MODE :You're not channel operator\r\n", 38, 0);
                    }
                } else if (cmds[2] == "-l") {
                    // 채널에 인원제한 해제
                    // 유저가 op인지 확인
                    if (_channels[cmds[1]].isClientOP(fd)) {
                        _channels[cmds[1]].setIsUserLimitSet(false);
                        send(fd, "MODE :-l\r\n", 10, 0);
                    } else {
                        send(fd, "482 MODE :You're not channel operator\r\n", 38, 0);
                    }
                } else if (cmds[2] == "+k") {
                    // 채널에 비밀번호 설정
                    // 유저가 op인지 확인
                    if (_channels[cmds[1]].isClientOP(fd)) {
                        _channels[cmds[1]].setIsPasswordSet(true);
                        _channels[cmds[1]].setPassword(cmds[3]);
                        send(fd, "MODE :+k\r\n", 9, 0);
                    } else {
                        send(fd, "482 MODE :You're not channel operator\r\n", 38, 0);
                    }
                } else if (cmds[2] == "-k") {
                    // 채널에 비밀번호 해제
                    // 유저가 op인지 확인
                    if (_channels[cmds[1]].isClientOP(fd)) {
                        _channels[cmds[1]].setIsPasswordSet(false);
                        _channels[cmds[1]].setPassword("");
                        send(fd, "MODE :-k\r\n", 10, 0);
                    } else {
                        send(fd, "482 MODE :You're not channel operator\r\n", 38, 0);
                    }
                } else {
                    send(fd, "472 MODE :Unknown mode\r\n", 26, 0);
                }
            }
        }
    }
}

void Server::cmdTopic(int fd, std::vector<std::string> cmds) {
    if (cmds.size() < 3)
        send_fd(fd, RPL_461_NEEDMOREPARAMS(_clients[fd].getNickName(), "TOPIC"));
    else {
        // 채널이름이 #으로 시작하지 않는 경우
        if (cmds[1][0] != '#')
            send_fd(fd, RPL_403_NOSUCHCHANNEL(_clients[fd].getNickName(), cmds[1]));
        else {
            // 채널이름이 #으로 시작하는 경우
            // 채널이 존재하는 경우
            if (_channels.find(cmds[1]) != _channels.end()) {
                // 토픽을 설정하는 경우
                if (cmds[2] != "") {

                    if (_channels[cmds[1]].getIsTopicProtected()) {
                        // 토픽이 보호되는 경우
                        // 유저가 op인지 확인
                        if (_channels[cmds[1]].isClientOP(fd)) {
                            // 토픽 설정
                            _channels[cmds[1]].setTopic(cmds[2]);
                            send(fd, "TOPIC :End of /NAMES list.\r\n", 27, 0);
                        } else {
                            send(fd, "482 TOPIC :You're not channel operator\r\n", 39, 0);
                        }
                    } else {
                        // 토픽 설정
                        _channels[cmds[1]].setTopic(cmds[2]);
                        send(fd, "TOPIC :End of /NAMES list.\r\n", 27, 0);
                    }
                } else {
                    // 토픽을 해제하는 경우
                    _channels[cmds[1]].setTopic(cmds[2]);
                    // 토픽 전송
                    send(fd, "332 TOPIC :End of /NAMES list.\r\n", 27, 0);
                }
            } else {
                // 채널이 존재하지 않는 경우
                send_fd(fd, RPL_403_NOSUCHCHANNEL(_clients[fd].getNickName(), cmds[1]));
            }
        }
    }
}

void Server::cmdKick(int fd, std::vector<std::string> cmds) {
    if (cmds.size() < 3)
        send_fd(fd, RPL_461_NEEDMOREPARAMS(_clients[fd].getNickName(), "KICK"));
    else {
        // 채널이름이 #으로 시작하지 않는 경우
        if (cmds[1][0] != '#')
            send_fd(fd, RPL_403_NOSUCHCHANNEL(_clients[fd].getNickName(), cmds[1]));
        else {
            // 채널이름이 #으로 시작하는 경우
            // 채널이 존재하는 경우
            if (_channels.find(cmds[1]) != _channels.end()) {
                // 유저가 op인지 확인
                if (_channels[cmds[1]].isClientOP(fd)) {
                    // 킥할 유저가 존재하는 경우
                    std::map<int, Client *>::iterator it;

                    for (it = _channels[cmds[1]].getClients().begin(); it != _channels[cmds[1]].getClients().end(); ++it) {
                        if (it->second->getNickName() == cmds[2]) {
                            // 킥할 유저 제거
                            _channels[cmds[1]].removeClient(it->first);
                            // 킥 메시지 전송
                            send(fd, "KICK :End of /NAMES list.\r\n", 27, 0);
                            break;
                        }
                    }

                    if (it == _channels[cmds[1]].getClients().end()) {
                        // 킥할 유저가 존재하지 않는 경우
                        send(fd, "441 KICK :They aren't on that channel\r\n", 40, 0);
                    }
                } else {
                    send(fd, "482 KICK :You're not channel operator\r\n", 39, 0);
                }
            } else {
                // 채널이 존재하지 않는 경우
                send_fd(fd, RPL_403_NOSUCHCHANNEL(_clients[fd].getNickName(), cmds[1]));
            }
        }
    }
}

void Server::cmdInvite(int fd, std::vector<std::string> cmds) {
    if (cmds.size() < 3)
        send_fd(fd, RPL_461_NEEDMOREPARAMS(_clients[fd].getNickName(), "INVITE"));
    else {
        // 채널이름이 #으로 시작하지 않는 경우
        if (cmds[1][0] != '#')
            send_fd(fd, RPL_403_NOSUCHCHANNEL(_clients[fd].getNickName(), cmds[1]));
        else {
            // 채널이름이 #으로 시작하는 경우
            // 채널이 존재하는 경우
            if (_channels.find(cmds[1]) != _channels.end()) {
                // 초대전용 채널인지 확인
                if (_channels[cmds[1]].getIsInviteOnly()) {
                    // 초대할 유저가 존재하는 경우
                    std::map<int, Client>::iterator it;

                    for (it = _clients.begin(); it != _clients.end(); ++it) {
                        if (it->second.getNickName() == cmds[2]) {
                            // 초대할 유저 추가
                            _channels[cmds[1]].addInviteClient(it->first);
                            // 초대 메시지 전송
                            send(fd, "INVITE :End of /NAMES list.\r\n", 28, 0);
                            break;
                        }
                    }

                    if (it == _clients.end()) {
                        // 초대할 유저가 존재하지 않는 경우
                        send(fd, "441 INVITE :They aren't on that channel\r\n", 41, 0);

                    }
                } else {
                    // 초대전용 채널이 아닌 경우
                    send(fd, "482 INVITE :Channel is not invite only\r\n", 41, 0);

                }
            } else {
                // 채널이 존재하지 않는 경우
                send_fd(fd, RPL_403_NOSUCHCHANNEL(_clients[fd].getNickName(), cmds[1]));
            }
        }
    }
}

void Server::cmdPing(int fd, std::vector<std::string> cmds) {
    if (cmds.size() < 2)
        send_fd(fd, RPL_461_NEEDMOREPARAMS(_clients[fd].getNickName(), "PING"));
    else {
        send_fd(fd, RPL_PONG(_clients[fd].getNickName()));
    }
}

void Server::cmdQuit(int fd, int i) {
    sendAll(fd, RPL_QUIT(_clients[fd].getNickName()));

    _lines[i].clear();
    close(fd);
    _pollFds.erase(_pollFds.begin() + i);
    for (std::map<std::string, Channel>::iterator it = _channels.begin(); it != _channels.end(); it++) {
        it->second.removeClient(fd);
    }
    _clients.erase(fd);
}