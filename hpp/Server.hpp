#ifndef SERVER_HPP
#define SERVER_HPP

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>

#include "StringUtils.hpp"
#include "Location.hpp"
#include <typeinfo>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>

class Server
{
	public:
		~Server();
		Server();
		Server(const Server &);
		Server & operator = (const Server &);
		std::map<std::string, bool>	_methods;
		std::vector<Location>		_locations;
		std::string					_root;
		std::string					_index;
		std::string					_server_name;
		int							_body_max_size;
		std::string					_error_404;
		std::string					_error_500;
		int							_port;
		struct  sockaddr_in			_addr;

		bool	get_method(const std::string &);
};

#endif

