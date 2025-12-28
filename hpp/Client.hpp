#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <iostream>
#include <sstream>
#include <ctime>
#include <map>

class Client
{		
	public:
		std::time_t	timeOut;
		int 		server_conf_key, fd, statuc_code;
		bool		is_dir,	end_request;
		std::string	buffer,	outbuf,	best_mach;
		std::map<std::string, std::string>	request;
		
		~Client();
		Client();
		Client(int);
		Client(const Client &);
		Client & operator = (const Client &);
};

#endif