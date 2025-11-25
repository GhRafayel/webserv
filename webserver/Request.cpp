#include "Request.hpp"

Request::~Request() {}

Request::Request() : StringUtils(), str_request() {}

Request::Request(std::map<int, Client>::iterator it) : StringUtils(), str_request() {(void)it;}

int	Request::to_read(int fd)
{
	char	buffer[1025];

	int n = recv(fd, buffer, 1024, 0);
	if (n)
	{
		buffer[n] = '\0';
		this->str_request = buffer;
	}
	return n;
}


bool    Request::end_of_request()
{
    if (this->str_request.length() < 4)
        return false;
    size_t pos = this->str_request.find("\r\n\r\n");
    if (pos != std::string::npos)
        return true;
    return false;
}

// void	Request::analize_request(std::map<int, Client>::iterator it)
// {
// 	int count_of_readed_chars = to_read(it->second.fd);

// 	if (count_of_readed_chars == 0)
// 		it->second.end_request = true;
// 	else if (count_of_readed_chars != -1 && end_of_request())
// 	{
// 		std::cout << "hello world" << std::endl;
// 	}
	
// }
