#include "../inc/Server.hpp"

Server::Server(int port, std::string pwd) : _pwd(pwd) {
	try {
		if ((_servFd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
			throw std::runtime_error("server socket does not generate");

		_servAddr.sin_family = AF_INET;
		_servAddr.sin_addr.s_addr = INADDR_ANY;
		_servAddr.sin_port = htons(port);

		if (bind(_servFd, (struct sockaddr*)&_servAddr, sizeof(_servAddr)) == -1)
			throw std::runtime_error("bind function fail");
	
		if (listen(_servFd, MAX_CLIENT) == -1)
			throw std::runtime_error("listen function fail");
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

	// _clients.push_back(Client(clientFd));

	return (true);
}

void Server::recvClient(int i) {
	int count = 0;
	int clientFd = _pollFds[i].fd;
	char buf[BUF_SIZE] = {0,};

	count = recv(clientFd, buf, BUF_SIZE, 0);

	if (count <=- 0) {
		if (count == 0)
			std::cout << "Client(fd: " << clientFd << ") is disconnected" << std::endl;
		else
			std::cout << "Error: recv function fail" << std::endl;
		close(clientFd);
		_pollFds.erase(_pollFds.begin() + i);
		return ;
	}

	_lines[i] += buf;
	std::string::size_type pos;

	while ((pos = _lines[i].find("\r\n")) != std::string::npos) {
		std::string line = _lines[i].substr(0, pos);
		std::vector<std::string> command = splitMsg(line);

		std::cout << "---- command ----\n" << line << std::endl;
		
		for (int j = 0; j < (int)(command.size()); j++)
			std::cout << "msg[" << j << "]: " << command[j] << std::endl;
		// execCommand(clientFd, command);
		_lines[i].erase(0, pos + 2);
	}
}

void Server::startServ() {
	std::cout << "server opened" << std::endl;

	while (poll(&_pollFds[0], _pollFds.size(), -1)) {
		if (_pollFds[0].revents & POLLIN)
			if (!this->acceptClient())
				std::cout << "Error: client socket does not generate" << std::endl; // 에러 처리
		for (int i = 1; i < (int)(_pollFds.size()); i++) {
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