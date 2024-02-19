#include "../inc/Client.hpp"

///// Constructor & Destructor /////

Client::Client() {
	_clientFd = -1;
	_clientFd = -1;
	ClientStatus _status = ClientStatus::NoPassword;
	_nickName = "*";
	_userName = "*";
	_hostName = "*";
	_realName = "*";
	_serverName = "*";
};

Client::~Client() {
};

Client::Client(int fd) : _clientFd(fd) {
	_clientFd = fd;
	ClientStatus _status = ClientStatus::NoPassword;
	_nickName = "*";
	_userName = "*";
	_hostName = "*";
	_realName = "*";
	_serverName = "*";
}

///// Get data /////

int Client::getClientFd() {
	return (_clientFd);
}

ClientStatus Client::getStatus() {
	if (!_isPasswordSet)
		return (ClientStatus::NoPassword);
	if (!_isNickSet)
		return (ClientStatus::NoNickname);
	if (!_isUserSet)
		return (ClientStatus::NoUsername);
	return (ClientStatus::LoggedIn);
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

void Client::setStatus(ClientStatus status) {
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
