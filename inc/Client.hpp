#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include "Define.hpp"

class Client {
private:
	///// 클라이언트 데이터 /////
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

	// 클라이언트의 파일 디스크립터
	int _clientFd;

	// 클라이언트의 상태
	int _status;

public:
	Client();

	~Client();

	Client(int fd);

	int getClientFd();
};

#endif