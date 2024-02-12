#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include "Define.hpp"

class Client {
	private:
		std::string _nickname;
		std::string _hostname;
		std::string _servname;
		std::string _realname;
		int _clientFd;
		int _status;
	public:
		Client(int fd);
		int getClientFd();
};

#endif