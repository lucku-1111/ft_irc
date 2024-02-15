#ifndef SERVER_HPP
#define SERVER_HPP

#include <poll.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <cstring>
#include <iostream>
#include <vector>
#include "Client.hpp"

#define BUF_SIZE 512
#define MAX_CLIENT 100

class Server {
	private:
		int _servFd;
		struct sockaddr_in _servAddr;
		std::vector<struct pollfd> _pollFds;
		std::string _pwd;
		std::vector<Client> _clients;
		std::string _lines[MAX_CLIENT];

	public:
		Server(int port, std::string pwd);

		void startServ();
		bool acceptClient();
		void recvClient(int i);
};

std::vector<std::string> splitMsg(std::string line);

#endif