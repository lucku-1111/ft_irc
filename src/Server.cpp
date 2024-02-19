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

///// Send Function /////
void Server::sendMsg(int fd, std::string msg) {
	// send 함수
	int ret = send(fd, msg.c_str(), msg.length(), 0);

	if (ret == -1)
		std::cerr << "send error : " << msg << std::endl;
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
		_clients[fd].setRealName(cmds[4]);
		_clients[fd].setIsUserSet(true);
	}
}
