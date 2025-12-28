#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <iostream>
#include <sstream>
#include <ctime>
#include <map>

class Client
{		
	public:
		std::time_t							timeOut;
		int 								server_conf_key;
		int									fd;
		int									statuc_code;
		bool								is_dir;
		bool								end_request;
		std::string							buffer;
		std::string							outbuf;
		std::string							best_mach;
		std::map<std::string, std::string>	request;
		
		~Client();
		Client();
		Client(int);
		Client(const Client &);
		Client & operator = (const Client &);
};

#endif