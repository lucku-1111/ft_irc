#include "../inc/Client.hpp"

Client::Client(int fd) : _clientFd(fd) {
	_nickname = "";
	_hostname = "";
	_servname = "";
	_realname = "";
}

int Client::getClientFd() {return (_clientFd);}