#include "Request.hpp"

Request::~Request() {}

Request::Request() {}

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
	if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1)
		throw std::string("fcntl failed");
}

std::string	Request::to_read(int fd)
{
	char buffer[1025];
	int n = recv(fd, buffer, 1024, 0);
	if (n )
	buffer[n] = '\0';
	std::string temp(buffer);
	return temp;
}

void	Request::analize_request(client & obj)
{
	size_t in = obj.buffer.find("\r\n");
	if (in != std::string::npos)
	{
		std::string first_line = obj.buffer.substr(0, in);
		std::vector<std::string> container = split(first_line, " ");
		for (size_t i = 0; i < container.size(); i++)
		{
			std::cout << container[i] << std::endl;
		}
		
		std::cout << first_line << std::endl;
	}

}