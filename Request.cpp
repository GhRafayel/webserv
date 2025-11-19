#include "Request.hpp"

Request::~Request() {}

int     Request::connect(int socket) {
	int fd = accept(socket, NULL, NULL);
	if (fd >= 0)
		std::cout << "Client connected successfully" << std::endl;
	else
		std::cout << "Client connection failed" << std::endl;
	return fd;
}

void		Request::nonBlock(int fd)
{
	int	fd = fcntl(fd, F_SETFL, O_NONBLOCK);
	if (fd == -1)
		throw std::string("fcntl failed");
}

std::string	Request::to_read(int fd)
{
	std::string temp;
	char *buffer[1025];

	int n = read(fd, buffer, 1024);
	if (n )
	buffer[n] = '\0';
	temp.append(buffer);
	return temp;
}