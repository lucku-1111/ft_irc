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
#include "Channel.hpp"
#include <map>

#define BUF_SIZE 512
#define MAX_CLIENT 100

class Client;
class Channel;

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

	void acceptClient();

	void recvClient(int i);

	///// Command Execution /////
	void executeCommand(int fd, std::vector<std::string> cmds, int idx);

	///// Send Functions /////
	void sendMsg(int fd, std::string msg);

	void sendMsgToChannel(std::string channelName, std::string msg);

	///// Command Functions /////
	void cmdPass(int fd, std::vector<std::string> cmds);

	void cmdNick(int fd, std::vector<std::string> cmds);

	void cmdUser(int fd, std::vector<std::string> cmds);

	void cmdJoin(int fd, std::vector<std::string> cmds);

	void cmdPart(int fd, std::vector<std::string> cmds);

	void cmdPrivMsg(int fd, std::vector<std::string> cmds);

	void cmdMode(int fd, std::vector<std::string> cmds);

	void cmdTopic(int fd, std::vector<std::string> cmds);

	void cmdInvite(int fd, std::vector<std::string> cmds);

	void cmdKick(int fd, std::vector<std::string> cmds);

	void cmdPing(int fd, std::vector<std::string> cmds);

	void cmdQuit(int fd, std::vector<std::string> cmds, int i);

	void sendMsgToChannel(int fd, std::string channelName, std::string msg);

};

std::vector<std::string> splitMsg(std::string line);

#endif