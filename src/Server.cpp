#include "../inc/Server.hpp"

Server::Server(int port, std::string pwd) : _pwd(pwd) {
	try {
		if ((_servFd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
			throw std::runtime_error("socket");

		_servAddr.sin_family = AF_INET;
		_servAddr.sin_addr.s_addr = INADDR_ANY;
		_servAddr.sin_port = htons(port);

		if (bind(_servFd, (struct sockaddr *)&_servAddr, sizeof(_servAddr)) ==
			-1)
			throw std::runtime_error("bind");

		if (listen(_servFd, MAX_CLIENT) == -1)
			throw std::runtime_error("listen");
	}
	catch (const std::runtime_error &e) {
		if (_servFd != -1)
			close(_servFd);
		std::cerr << "Error: " << e.what() << std::endl;
		exit(1);
	}
	_pollFds.push_back((struct pollfd){_servFd, POLLIN, 0});
}

bool Server::acceptClient() {
	struct sockaddr_in clientAddr;
	socklen_t clientLen = sizeof(clientAddr);
	int clientFd = accept(_servFd, (struct sockaddr *)&clientAddr, &clientLen);
	if (clientFd == -1)
		return (false);

	_pollFds.push_back((struct pollfd){clientFd, POLLIN | POLLHUP, 0});
	_pollFds[0].revents = 0;
	fcntl(clientFd, F_SETFL, O_NONBLOCK);

	_clients.push_back(Client(clientFd));

	return (true);
}

bool Server::recvClient(int i) {
	int count = 0;
	char buf[BUF_SIZE] = {0,};

	count = recv(_pollFds[i].fd, buf, BUF_SIZE, 0)) > 0)
	_lines[i].append(buf);
	std::memset(buf, 0, BUF_SIZE);


	if (count == 0) {
	} // 클라이언트 연결 종료 처리

	// 받은 명령어 처리
	std::cout << _lines[i] << std::endl;
}

void Server::startServ() {
	std::cout << "server opened" << std::endl;

	while (poll(&_pollFds[0], _pollFds.size(), -1)) {
		if (_pollFds[0].revents & POLLIN) {
			if (!this->acceptClient())
				std::cout << "err" << std::endl; // 에러핸들
		}
		for (int i = 1; i < (int)(_pollFds.size()); i++) {
			if (_pollFds[i].revents & (POLLIN | POLLHUP)) {
				std::cout << "i: " << i << std::endl;
				if (!this->recvClient(i))
					std::cout << "err" << std::endl; // 에러핸들
				// _lines[i].clear();
			}
		}
	}
}

// ToDo : 클라이언트 생성은 poll에서 새로운 클라이언트가 들어왔을 때만 생성?
// ToDo : 클라이언트 소켓을 닫을 때 클라이언트 객체도 삭제해야하는지?

///// Command Execution /////

void Server::executeCommand(int fd, std::vector<std::string> cmds) {
	// 명령어 처리
	// 명령어 벡터 string으로 받아오기
	std::string cmdType = cmds[0];

	while (1) {
		// 클라이언트의 상태에 따라 명령을 처리
		if (_clients[fd].getStatus() == ClientStatus::NoPassword) {
			// 비밀번호가 없는 상태에서 pass 명령어인 경우
			if (cmdType == "pass" || cmdType == "PASS")
				// pass 명령어 처리
				cmdPass(fd, cmds);
			else
				// 비밀번호가 없는 상태에서 pass 명령어가 아닌 경우
				send(fd,
					 "CheckIn cmdType must be in order : |PASS| -> |NICK| -> |USER| \r\n",
					 61, 0);
		} else if (_clients[fd].getStatus() == ClientStatus::NoNickname) {
			// 비밀번호는 있고 닉네임이 없는 상태에서 pass, nick 명령어인 경우
			if (cmdType == "pass" || cmdType == "PASS")
				// pass 명령어 처리 : pass는 마지막에 들어온 pass 기준으로 저장
				cmdPass(fd, cmds);
			else if (cmdType == "nick" || cmdType == "NICK")
				// nick 명령어 처리
				cmdNick(fd, cmds);
			else
				// 비밀번호는 있고 닉네임이 없는 상태에서 pass, nick 명령어가 아닌 경우
				send(fd,
					 "CheckIn cmdType must be in order : |PASS| -> |NICK| -> |USER| \r\n",
					 61, 0);
		} else if (_clients[fd].getStatus() == ClientStatus::NoUsername) {
			// 유저네임이 없는 상태에서 pass, nick, user 명령어인 경우
			if (cmdType == "pass" || cmdType == "PASS")
				cmdPass(fd, cmds);
			else if (cmdType == "nick" || cmdType == "NICK")
				cmdNick(fd, cmds);
			else if (cmdType == "user" || cmdType == "USER") {
				cmdUser(fd, cmds);
				// welcome 메시지 전송
				send(fd, "001 :Welcome to the Internet Relay Network\r\n", 42,
					 0);
			}
		} else if (_clients[fd].getStatus() == ClientStatus::LoggedIn) {
			// 로그인 상태에서 명령어 처리
			if (cmdType == "pass" || cmdType == "PASS")
				send(fd, "462 PASS :You may not reregister\r\n", 33, 0);
			else if (cmdType == "nick" || cmdType == "NICK")
				cmdNick(fd, cmds);
			else if (cmdType == "user" || cmdType == "USER")
				send(fd, "462 USER :You may not reregister\r\n", 33, 0);
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
				cmdQuit(fd, cmds);
			else if (cmdType == "ping" || cmdType == "PING")
				cmdPing(fd, cmds);
		}
	}

}

///// Send Functions /////
void Server::sendMsg(int fd, std::string msg) {
	// send 함수
	int ret = send(fd, msg.c_str(), msg.length(), 0);

	if (ret == -1)
		std::cerr << "send error : " << msg << std::endl;
}

void
Server::sendMsgToChannel(int fd, std::string channelName, std::string msg) {
	// 채널에 메시지 전송
}

///// Command Functions /////
void Server::cmdPass(int fd, std::vector<std::string> cmds) {
	if (cmds.size() < 2)
		send(fd, "461 PASS :Not enough parameters\r\n", 31, 0);
	else {
		_clients[fd].setPassword(cmds[1]);
		_clients[fd].setIsPasswordSet(true);
	}
}

void Server::cmdNick(int fd, std::vector<std::string> cmds) {
	if (cmds.size() < 2)
		send(fd, "461 NICK :Not enough parameters\r\n", 31, 0);
	else {
		_clients[fd].setNickName(cmds[1]);
		_clients[fd].setIsNickSet(true);
	}
}

void Server::cmdUser(int fd, std::vector<std::string> cmds) {
	if (cmds.size() < 5)
		send(fd, "461 USER :Not enough parameters\r\n", 31, 0);
	else {
		_clients[fd].setUserName(cmds[1]);
		// 호스트네임과 서버네임은 서버가 맘대로 설정??
		_clients[fd].setRealName(cmds[4]);
		_clients[fd].setIsUserSet(true);
	}
}

void Server::cmdJoin(int fd, std::vector<std::string> cmds) {
	if (cmds.size() < 2)
		send(fd, "461 JOIN :Not enough parameters\r\n", 31, 0);
	else {
		// 채널이름이 #으로 시작하지 않는 경우
		if (cmds[1][0] != '#')
			send(fd, "403 JOIN :No such channel\r\n", 27, 0);
		else {
			// 채널이름이 #으로 시작하는 경우
			// 채널이름이 이미 존재하는 경우
			if (_channels.find(cmds[1]) != _channels.end()) {

				// 채널에 클라이언트 추가
				_channels[cmds[1]].addClient(fd, &_clients[fd]);

				// 클라이언트에 채널 추가
				_clients[fd].addChannel(cmds[1]);

				// 채널에 메시지 전송
				send(fd, "JOIN :End of /NAMES list.\r\n", 25, 0);
			} else {

				// 채널이름이 존재하지 않는 경우
				// 채널 생성
				Channel channel(cmds[1]);

				// 서버 채널리스트에 채널 추가
				_channels[cmds[1]] = channel;

				// 채널에 클라이언트 추가
				channel.addClient(fd, &_clients[fd]);

				// 클라이언트에 채널 추가
				_clients[fd].addChannel(cmds[1]);

			}
		}
	}
}

void Server::cmdPart(int fd, std::vector<std::string> cmds) {
	if (cmds.size() < 2)
		send(fd, "461 PART :Not enough parameters\r\n", 31, 0);
	else {
		// 채널이름이 #으로 시작하지 않는 경우
		if (cmds[1][0] != '#')
			send(fd, "403 PART :No such channel\r\n", 27, 0);
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
													"PART :End of /NAMES list.\r\n");
			} else {

				// 채널이름이 존재하지 않는 경우
				send(fd, "403 PART :No such channel\r\n", 27, 0);
			}
		}
	}
}

void Server::cmdPrivMsg(int fd, std::vector<std::string> cmds) {
	if (cmds.size() < 3)
		send(fd, "461 PRIVMSG :Not enough parameters\r\n", 35, 0);
	else {
		// 수신자가 채널인 경우
		if (cmds[1][0] == '#') {
			// 채널이 존재하는 경우
			if (_channels.find(cmds[1]) != _channels.end()) {
				// 채널에 메시지 전송
				_channels[cmds[1]].sendToAllClients(fd, cmds[2]);
			} else {
				// 채널이 존재하지 않는 경우
				send(fd, "403 PRIVMSG :No such channel\r\n", 31, 0);
			}
		} else {
			// 수신자가 클라이언트인 경우
			// 수신자가 존재하는 경우
			if (_clients.find(cmds[1]) != _clients.end()) {
				// 수신자에게 메시지 전송
				send(_clients[cmds[1]].getFd(), cmds[2], cmds[2].length(), 0);
			} else {
				// 수신자가 존재하지 않는 경우
				send(fd, "401 PRIVMSG :No such nick/channel\r\n", 35, 0);
			}
		}
	}
}

void Server::cmdMode(int fd, std::vector<std::string> cmds) {
	if (cmds.size() < 3)
		send(fd, "461 MODE :Not enough parameters\r\n", 31, 0);
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
						send(fd, "482 MODE :You're not channel operator\r\n",
							 38, 0);
					}
				} else if (cmds[2] == "-i") {
					// 채널에 초대전용 해제
					// 유저가 op인지 확인
					if (_channels[cmds[1]].isClientOP(fd)) {
						_channels[cmds[1]].setIsInviteOnly(false);
						send(fd, "MODE :-i\r\n", 10, 0);
					} else {
						send(fd, "482 MODE :You're not channel operator\r\n",
							 38, 0);
					}
				} else if (cmds[2] == "+o") {
					if (cmds.size() < 4) {
						// 닉네임이 들어왔는지 확인
						send(fd, "461 MODE :Not enough parameters\r\n", 31, 0);
					} else if (_channels[cmds[1]].isClientOP(fd)) {
						// 유저가 op인지 확인
						// 유저에게 op 권한 부여
						_channels[cmds[1]].addClientToOPList(fd);
						send(fd, "MODE :+o\r\n", 9, 0);
					} else {
						send(fd, "482 MODE :You're not channel operator\r\n",
							 38, 0);
					}
				} else if (cmds[2] == "-o") {
					if (cmds.size() < 4) {
						// 닉네임이 들어왔는지 확인
						send(fd, "461 MODE :Not enough parameters\r\n", 31, 0);
					} else if (_channels[cmds[1]].isClientOP(fd)) {
						// 유저에게 op 권한 해제
						// 유저가 op인지 확인
						_channels[cmds[1]].removeClientFromOPList(fd);
						send(fd, "MODE :-o\r\n", 10, 0);
					} else {
						send(fd, "482 MODE :You're not channel operator\r\n",
							 38, 0);
					}
				} else if (cmds[2] == "+t") {
					// op만 토픽 설정 가능
					// 유저가 op인지 확인
					if (_channels[cmds[1]].isClientOP(fd)) {
						_channels[cmds[1]].setIsTopicProtected(true);
						send(fd, "MODE :+t\r\n", 9, 0);
					} else {
						send(fd, "482 MODE :You're not channel operator\r\n",
							 38, 0);
					}
				} else if (cmds[2] == "-t") {
					// 채널에 토픽 해제
					// 유저가 op인지 확인
					if (_channels[cmds[1]].isClientOP(fd)) {
						_channels[cmds[1]].setIsTopicProtected(false);
						send(fd, "MODE :-t\r\n", 10, 0);
					} else {
						send(fd, "482 MODE :You're not channel operator\r\n",
							 38, 0);
					}
				} else if (cmds[2] == "+l") {
					// 채널에 인원제한 설정
					// 유저가 op인지 확인
					if (_channels[cmds[1]].isClientOP(fd)) {
						_channels[cmds[1]].setIsUserLimitSet(true);
						send(fd, "MODE :+l\r\n", 9, 0);
					} else {
						send(fd, "482 MODE :You're not channel operator\r\n",
							 38, 0);
					}
				} else if (cmds[2] == "-l") {
					// 채널에 인원제한 해제
					// 유저가 op인지 확인
					if (_channels[cmds[1]].isClientOP(fd)) {
						_channels[cmds[1]].setIsUserLimitSet(false);
						send(fd, "MODE :-l\r\n", 10, 0);
					} else {
						send(fd, "482 MODE :You're not channel operator\r\n",
							 38, 0);
					}
				} else if (cmds[2] == "+k") {
					// 채널에 비밀번호 설정
					// 유저가 op인지 확인
					if (_channels[cmds[1]].isClientOP(fd)) {
						_channels[cmds[1]].setIsPasswordSet(true);
						_channels[cmds[1]].setPassword(cmds[3]);
						send(fd, "MODE :+k\r\n", 9, 0);
					} else {
						send(fd, "482 MODE :You're not channel operator\r\n",
							 38, 0);
					}
				} else if (cmds[2] == "-k") {
					// 채널에 비밀번호 해제
					// 유저가 op인지 확인
					if (_channels[cmds[1]].isClientOP(fd)) {
						_channels[cmds[1]].setIsPasswordSet(false);
						_channels[cmds[1]].setPassword("");
						send(fd, "MODE :-k\r\n", 10, 0);
					} else {
						send(fd, "482 MODE :You're not channel operator\r\n",
							 38, 0);
					}
				} else {
					send(fd, "472 MODE :Unknown mode\r\n", 26, 0);
				}
			}
		}
	}
}
