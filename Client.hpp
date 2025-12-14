#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <iostream>
#include <sstream>
#include <ctime>

class Client
{		
	public:
		std::time_t timeOut;
		int			server_conf_key;
		int			fd;
		int			statuc_code;
		bool		end_request;
		std::string	buffer;
		std::string	outbuf;
		std::string	best_mach;
		
		~Client();
		Client();
		Client(int);
		Client(const Client &);
		Client & operator = (const Client &);
};

#endif