#include "../inc/Server.hpp"

bool checkPort(char* str) {
	int port = atoi(str);

	if (port < 1024 || port > 65535)
		return (false);
	return (true);
}

bool checkPwd(std::string pwd) {
	if (pwd[0] == '#' || pwd[0] == ':')
		return (false);
	return (true);
}

int main(int ac, char** av) {
	if (ac != 3 || !checkPort(av[1]) || !checkPwd(av[2])) {
		std::cout << "Invalid arguments" << std::endl;
		return (1);
	}

	Server serv(atoi(av[1]), av[2]);

	serv.startServ();

	return (0);
}