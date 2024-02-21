#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include "Channel.hpp"
#include "Server.hpp"
#include <map>

enum ClientStatus {
	NoPassword,
	NoNickname,
	NoUsername,
	LoggedIn
};

class Channel;

class Client {
private:
	///// 클라이언트 데이터 /////

	// 클라이언트의 파일 디스크립터
	int _clientFd;

	// 클라이언트의 상태
	ClientStatus _status;

	// 클라이언트의 닉네임
	std::string _nickName;

	// 클라이언트의 유저네임
	std::string _userName;

	// 클라이언트의 호스트네임
	std::string _hostName;

	// 클라이언트의 리얼네임
	std::string _realName;

	// 클라이언트의 서버네임
	std::string _serverName;

	// 클라이언트가 속한 채널
	std::map<std::string, Channel *> _clientChannels;

	///// 클라이언트의 명령어 수신 여부 /////
	// PASS 명령어가 수신되었는지 여부
	bool _isPasswordSet;

	// NICK 명령어가 수신되었는지 여부
	bool _isNickSet;

	// USER 명령어가 수신되었는지 여부
	bool _isUserSet;


public:

	///// Constructor & Destructor /////
	Client();

	~Client();

	Client(int fd);

	///// Getter /////

	// get client's file descriptor
	int getClientFd();

	// get client's status
	ClientStatus getStatus();

	// get client's nickname
	const std::string getNickName();

	// get client's username
	std::string getUserName();

	// get client's realname
	std::string getRealName();

	// get client's hostname
	std::string getHostName();

	// get client's servername
	std::string getServerName();

	std::map<std::string, Channel *> getClientChannels();

	///// Setter /////
	// set client's file descriptor
	void setClientFd(int fd);

	// set client's status
	void setStatus(ClientStatus status);

	// set client's nickname
	void setNickName(std::string nickName);

	// set client's username
	void setUserName(std::string userName);

	// set client's realname
	void setRealName(std::string realName);

	// set client's hostname
	void setHostName(std::string hostName);

	// set client's servername
	void setServerName(std::string serverName);

	// set PASS command received
	void setIsPasswordSet(bool isPasswordSet);

	// set NICK command received
	void setIsNickSet(bool isNickSet);

	// set USER command received
	void setIsUserSet(bool isUserSet);

	// add channel to client
	void addChannel(std::string channelName, Channel *channel);

	// remove channel from client
	void removeChannel(std::string channelName);

};

#endif