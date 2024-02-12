#include "../inc/Server.hpp"

bool check_port(char* str) {
	int port = atoi(str);

	if (port < 1024 || port > 65535)
		return (false);
	return (true);
}

int main(int ac, char** av) {
	int (ac != 3 || !check_port(av[1])) {
		std::cout << "Invalid arguments" << std::endl;
		return (1);
	}

	Server serv(atoi(av[1]), av[2]);

	serv.startServ();

	return (0);
}