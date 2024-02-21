#include "../inc/Channel.hpp"
#include "../inc/msgs.hpp"

///// Constructor & Destructor /////
Channel::Channel() {
	_channelName = "";
	_password = "";
	_topic = "";
	_userLimit = 100;

	_isPasswordSet = false;
	_isInviteOnly = false;
	_isTopicProtected = false;
	_isUserLimitSet = false;
}

Channel::Channel(std::string channelName) {
	_channelName = channelName;
	_password = "";
	_topic = "";
	_userLimit = 100;

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

	std::cout << "::::::::: erase" << std::endl;
	// 채널에서 클라이언트를 제거
	_clients.erase(fd);

	std::cout << "::::::::: removeClientFromOPList" << std::endl;
	// op권한이 있었다면 제거
	removeClientFromOPList(fd);


	std::cout << "::::::::: removeClientFromInviteList" << std::endl;
	// 초대된 클라이언트 목록에서 제거
	removeClientFromInviteList(fd);

}

void Channel::removeClientFromInviteList(int fd) {
	std::vector<int>::iterator it = std::find(_inviteList.begin(),
											  _inviteList.end(), fd);

	// 초대된 클라이언트 목록에서 제거
	if (it != _inviteList.end())
		_inviteList.erase(it);
}

void Channel::addClientToOPList(int fd) {
	_operatorList.push_back(fd);
}

void Channel::removeClientFromOPList(int fd) {
	std::vector<int>::iterator it = std::find(_operatorList.begin(),
											  _operatorList.end(), fd);

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

	// input으로 받은 fd를 제외한 모든 클라이언트에게 메시지를 보냄
	for (it = _clients.begin(); it != _clients.end(); it++) {
		if (it->first != fd)
			send_fd(it->first, message);
	}
}

bool Channel::isFdInOPList(int fd) {
	std::vector<int>::iterator it = std::find(_operatorList.begin(),
											  _operatorList.end(), fd);

	// op권한이 있는지 확인
	if (it != _operatorList.end())
		return true;
	return false;
}

bool Channel::isFdInInviteList(int fd) {
	std::vector<int>::iterator it = std::find(_inviteList.begin(),
											  _inviteList.end(), fd);

	// 초대된 클라이언트인지 확인
	if (it != _inviteList.end())
		return true;
	return false;
}

std::string Channel::getChannelClients() {
	std::string ret = "";

	std::map<int, Client *>::iterator it;
	for (it = _clients.begin(); it != _clients.end(); it++) {

		// op인 경우 @ 추가
		if (isFdInOPList(it->first))
			ret += "@";

		ret += it->second->getNickName() + " ";
	}
	return ret;
}

bool Channel::isNickInChannel(std::string nick) {
	std::map<int, Client *>::iterator it;
	for (it = _clients.begin(); it != _clients.end(); it++) {
		if (it->second->getNickName() == nick)
			return true;
	}
	return false;
}

bool Channel::isFdInChannel(int fd) {
	std::map<int, Client *>::iterator it;
	for (it = _clients.begin(); it != _clients.end(); it++) {
		if (it->first == fd)
			return true;
	}
	return false;
}