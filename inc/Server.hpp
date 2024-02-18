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

	///// Command Execution /////
	void executeCommand(int fd, std::vector<std::string> cmds);

	///// Command Functions /////
	void commandPass(int fd, std::vector<std::string> cmds);

	void commandNick(int fd, std::vector<std::string> cmds);

	void commandUser(int fd, std::vector<std::string> cmds);

	void commandJoin(int fd, std::vector<std::string> cmds);

	void commandPart(int fd, std::vector<std::string> cmds);

	void commandQuit(int fd, std::vector<std::string> cmds);

	void commandTopic(int fd, std::vector<std::string> cmds);

	void commandInvite(int fd, std::vector<std::string> cmds);

	void commandKick(int fd, std::vector<std::string> cmds);

	void commandPrivmsg(int fd, std::vector<std::string> cmds);

	void commandMode(int fd, std::vector<std::string> cmds);

	void commandPing(int fd, std::vector<std::string> cmds);

	void commandPong(int fd, std::vector<std::string> cmds);


};

#endif