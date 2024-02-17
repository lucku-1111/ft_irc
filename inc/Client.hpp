#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include "Define.hpp"

class Client {
private:
	///// 클라이언트 데이터 /////

	// 클라이언트의 파일 디스크립터
	int _clientFd;

	// 클라이언트의 상태
	int _status;

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


public:

	///// Constructor & Destructor /////
	Client();

	~Client();

	Client(int fd);

	///// Getter /////

	// get client's file descriptor
	int getClientFd();

	// get client's status
	int getStatus();

	// get client's nickname
	std::string getNickName();

	// get client's username
	std::string getUserName();

	// get client's realname
	std::string getRealName();

	// get client's hostname
	std::string getHostName();

	// get client's servername
	std::string getServerName();

	///// Setter /////
	// set client's file descriptor
	void setClientFd(int fd);

	// set client's status
	void setStatus(int status);

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


};

#endif