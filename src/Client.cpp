#include "../inc/Client.hpp"

///// Constructor & Destructor /////

Client::Client() {
};

Client::~Client() {
};

Client::Client(int fd) : _clientFd(fd) {

}

///// Get data /////

int Client::getClientFd() {
	return (_clientFd);
}

int Client::getStatus() {
	return (_status);
}

std::string Client::getNickName() {
	return (_nickName);
}

std::string Client::getUserName() {
	return (_userName);
}

std::string Client::getRealName() {
	return (_realName);
}

std::string Client::getHostName() {
	return (_hostName);
}

std::string Client::getServerName() {
	return (_serverName);
}

///// Set data /////

void Client::setClientFd(int fd) {
	_clientFd = fd;
}

void Client::setStatus(int status) {
	_status = status;
}

void Client::setNickName(std::string nick) {
	_nickName = nick;
}

void Client::setUserName(std::string user) {
	_userName = user;
}

void Client::setRealName(std::string real) {
	_realName = real;
}

void Client::setHostName(std::string host) {
	_hostName = host;
}

void Client::setServerName(std::string server) {
	_serverName = server;
}
