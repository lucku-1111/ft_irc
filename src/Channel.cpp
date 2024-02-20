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


