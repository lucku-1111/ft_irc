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
#include <map>

#include "Client.hpp"
#include "Channel.hpp"

#define BUF_SIZE 512
#define MAX_CLIENT 100

class Server {
private:

	int _servFd;
	struct sockaddr_in _servAddr;
	std::vector<struct pollfd> _pollFds;
	std::string _pwd;
	std::string _lines[MAX_CLIENT];

	///// Server Database /////

	// 클라이언트들의 정보를 저장하는 맵
	std::map<int, Client> _clients;

	// 채널들의 정보를 저장하는 맵
	std::map<std::string, Channel> _channels;


public:
	Server(int port, std::string pwd);

	void startServ();

	bool acceptClient();

	bool recvClient(int i);
};

#endif