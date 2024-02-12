#include "../inc/Client.hpp"

Client::Client(int fd) : _clientFd(fd) {}

int Client::getClientFd() {return (_clientFd);}