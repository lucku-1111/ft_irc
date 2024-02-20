#include "../inc/Channel.hpp"


///// Constructor & Destructor /////
Channel::Channel() {
	_channelName = "";
	_password = "";
	_topic = "";
	_userLimit = 50;

	_isPasswordSet = false;
	_isInviteOnly = false;
	_isTopicProtected = false;
	_isUserLimitSet = false;
}

Channel::Channel(std::string channelName) {
	_channelName = channelName;
	_password = "";
	_topic = "";
	_userLimit = 50;

	_isPasswordSet = false;
	_isInviteOnly = false;
	_isTopicProtected = false;
	_isUserLimitSet = false;
}

Channel::~Channel() {
}

///// Getter /////
std::string Channel::getChannelName() {
	return _channelName;
}

std::map<int, Client *> Channel::getClients() {
	return _clients;
}

std::vector<int> Channel::getInviteList() {
	return _inviteList;
}

std::vector<int> Channel::getOperatorList() {
	return _operatorList;
}

std::string Channel::getPassword() {
	return _password;
}

std::string Channel::getTopic() {
	return _topic;
}

int Channel::getUserLimit() {
	return _userLimit;
}

bool Channel::getIsPasswordSet() {
	return _isPasswordSet;
}

bool Channel::getIsInviteOnly() {
	return _isInviteOnly;
}

bool Channel::getIsTopicProtected() {
	return _isTopicProtected;
}

bool Channel::getIsUserLimitSet() {
	return _isUserLimitSet;
}


///// Setter /////
void Channel::setChannelName(std::string channelName) {
	_channelName = channelName;
}

void Channel::setPassword(std::string password) {
	_password = password;
	_isPasswordSet = true;
}

void Channel::setTopic(std::string topic) {
	_topic = topic;
}

void Channel::setUserLimit(int limit) {
	_userLimit = limit;
	_isUserLimitSet = true;
}

void Channel::setIsPasswordSet(bool isPasswordSet) {
	_isPasswordSet = isPasswordSet;
}

void Channel::setIsInviteOnly(bool isInviteOnly) {
	_isInviteOnly = isInviteOnly;
}

void Channel::setIsTopicProtected(bool isTopicProtected) {
	_isTopicProtected = isTopicProtected;
}

void Channel::setIsUserLimitSet(bool isUserLimitSet) {
	_isUserLimitSet = isUserLimitSet;
}

///// Method /////
void Channel::addClient(int fd, Client *client) {
	_clients[fd] = client;

	// 채널에 처음 들어온 클라이언트는 op권한을 가짐
	if (_clients.size() == 1)
		_operatorList.push_back(fd);
}

void Channel::removeClient(int fd) {
	std::map<int, Client *>::iterator it = _clients.find(fd);

	// 채널에서 클라이언트를 제거
	if (it != _clients.end())
		_clients.erase(it);

	// op권한이 있었다면 제거
	removeClientFromOPList(fd);
}

void Channel::addClientToOPList(int fd) {
	_operatorList.push_back(fd);
}

void Channel::removeClientFromOPList(int fd) {
	std::vector<int>::iterator it = std::find(_operatorList.begin(), _operatorList.end(), fd);

	// op권한이 있었다면 제거
	if (it != _operatorList.end())
		_operatorList.erase(it);
}

void Channel::addInviteClient(int fd) {
	_inviteList.push_back(fd);
}

void Channel::addOperator(int fd) {
	_operatorList.push_back(fd);
}

void Channel::sendToAllClients(int fd, std::string message) {
	std::map<int, Client *>::iterator it;

	// 자신을 제외한 모든 클라이언트에게 메시지를 보냄
	for (it = _clients.begin(); it != _clients.end(); it++) {
		if (it->first != fd)
			send(it->first, message.c_str(), message.length(), 0);
	}
}

bool Channel::isClientOP(int fd) {
	std::vector<int>::iterator it = std::find(_operatorList.begin(), _operatorList.end(), fd);

	// op권한이 있는지 확인
	if (it != _operatorList.end())
		return true;
	return false;
}

bool Channel::isClientInvited(int fd) {
	std::vector<int>::iterator it = std::find(_inviteList.begin(), _inviteList.end(), fd);

	// 초대된 클라이언트인지 확인
	if (it != _inviteList.end())
		return true;
	return false;
}




