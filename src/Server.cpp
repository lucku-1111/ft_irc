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
	std::string command = cmds[0];

	while (1) {
		// 클라이언트의 상태에 따라 명령을 처리
		if (_clients[fd].getStatus() == ClientState::NoPassword) {
			// 비밀번호가 없는 상태에서 pass 명령어인 경우
			if (command == "pass" || command == "PASS")
				commandPass(fd, cmds);
			else
				// 비밀번호가 없는 상태에서 pass 명령어가 아닌 경우
				addBuffer("CheckIn command is |PASS| -> |NICK| -> |USER| \r\n",
						  fd);
		} else if (_clients[fd].getStatus() == ClientState::NoNickname) {
			// 닉네임이 없는 상태에서 pass, nick 명령어인 경우
			if (command == "pass" || command == "PASS")
				commandPass(fd, cmds);
			else if (command == "nick" || command == "NICK")
				commandNick(fd, cmds);
			else
				addBuffer("CheckIn command is |PASS| -> |NICK| -> |USER| \r\n",
						  fd);
		} else if (_clients[fd].getStatus() == ClientState::NoUsername) {
			// 유저네임이 없는 상태에서 pass, nick, user 명령어인 경우
			if (command == "pass" || command == "PASS")
				commandPass(fd, cmds);
			else if (command == "user" || command == "USER")
				commandUser(fd, cmds);
			else if (command == "nick" || command == "NICK")
				commandNick(fd, cmds);
		} else if (_clients[fd].getStatus() == ClientState::LoggedIn) {
			// 로그인 상태에서 명령어 처리
			if (command == "quit" || command == "QUIT")
				commandQuit(fd, cmds);
			else if (command == "join" || command == "JOIN")
				commandJoin(fd, cmds);
			else if (command == "nick" || command == "NICK")
				commandNick(fd, cmds);
			else if (command == "pass" || command == "PASS")
				commandPass(fd, cmds);
			else if (command == "user" || command == "USER")
				commandUser(fd, cmds);
			else if (command == "part" || command == "PART")
				commandPart(fd, cmds);
			else if (command == "privmsg" || command == "PRIVMSG")
				commandPrivmsg(fd, cmds);
			else if (command == "kick" || command == "KICK")
				commandKick(fd, cmds);
			else if (command == "invite" || command == "INVITE")
				commandInvite(fd, cmds);
			else if (command == "topic" || command == "TOPIC")
				commandTopic(fd, cmds);
			else if (command == "mode" || command == "MODE")
				commandMode(fd, cmds);
			else if (command == "ping" || command == "PING")
				commandPing(fd, cmds);
			else if (command == "pong" || command == "PONG")
				commandPong(fd, cmds);
		}
	}


}