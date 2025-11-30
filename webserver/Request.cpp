#include "Request.hpp"

Request::~Request() {}

Request::Request(Client & obj) : StringUtils(), client_ref(obj) {
	analize_request();
}

void	Request::analize_request()
{
	std::vector<std::string> req = split(client_ref.buffer, "\n", true);
	std::vector<std::string> header = split(req[0], " ", true);
	this->method = header[0];
	this->path = header[1];
	this->protocol = header[2];

	for (size_t i = 1; i < req.size(); i++)
	{
		std::vector<std::string> line = split(req[i], " ", true);

		if (line.size() == 2)
		{
			request.insert(std::make_pair(line[0], line[1]));
		}
	}
}
std::string Request::getProtocol(){
	return this->protocol;
};
std::string	Request::getMethod(){
	return this->method;	
};
std::string	Request::getPath(){
	return this->path;	
};

std::map<std::string, std::string> & Request::getRequest() {
	return this->request;
}

