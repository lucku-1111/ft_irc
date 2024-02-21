#include "../inc/Client.hpp"

///// Constructor & Destructor /////

Client::Client() {
	_clientFd = -1;
	_clientFd = -1;
	_status = NoPassword;
	_nickName = "*";
	_userName = "*";
	_hostName = "*";
	_realName = "*";
	_serverName = "*";
	_isPasswordSet = false;
	_isNickSet = false;
	_isUserSet = false;
};

Client::~Client() {
};

Client::Client(int fd) : _clientFd(fd) {
	_clientFd = fd;
	_status = NoPassword;
	_nickName = "*";
	_userName = "*";
	_hostName = "*";
	_realName = "*";
	_serverName = "*";
	_isPasswordSet = false;
	_isNickSet = false;
	_isUserSet = false;
}

///// Get data /////

int Client::getClientFd() {
	return (_clientFd);
}

ClientStatus Client::getStatus() {
	if (!_isPasswordSet)
		return (NoPassword);
	if (!_isNickSet)
		return (NoNickname);
	if (!_isUserSet)
		return (NoUsername);
	return (LoggedIn);
}

const std::string Client::getNickName() {
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

std::map<std::string, Channel *> Client::getClientChannels() {
	return (_clientChannels);
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

void Client::setIsPasswordSet(bool isPasswordSet) {
	_isPasswordSet = isPasswordSet;
}

void Client::setIsNickSet(bool isNickSet) {
	_isNickSet = isNickSet;
}

void Client::setIsUserSet(bool isUserSet) {
	_isUserSet = isUserSet;
}

void Client::addChannel(std::string channelName, Channel *channel) {
	_clientChannels.insert(
			std::pair<std::string, Channel *>(channelName, channel));
}

void Client::removeChannel(std::string channelName) {
	_clientChannels.erase(channelName);
}