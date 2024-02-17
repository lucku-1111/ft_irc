#include "../inc/Server.hpp"

Server::Server(int port, std::string pwd) : _pwd(pwd) {
	try {
		if ((_servFd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
			throw std::runtime_error("socket");

		_servAddr.sin_family = AF_INET;
		_servAddr.sin_addr.s_addr = INADDR_ANY;
		_servAddr.sin_port = htons(port);

		if (bind(_servFd, (struct sockaddr*)&_servAddr, sizeof(_servAddr)) == -1)
			throw std::runtime_error("bind");
	
		if (listen(_servFd, MAX_CLIENT) == -1)
			throw std::runtime_error("listen");
	}
	catch (const std::runtime_error& e) {
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
	int clientFd = accept(_servFd, (struct sockaddr*)&clientAddr, &clientLen);
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



	if (count == 0) {} // 클라이언트 연결 종료 처리

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

void Server::executeCommand(int fd, std::vector<std::string> cmd) {
	// 명령어 처리
	// Todo : 명령어 처리
}