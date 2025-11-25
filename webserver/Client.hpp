#pragma once

#include <iostream>

class Client
{
	private:
		Client();
		Client & operator = (const Client &);
	public:
		int			server_conf_key;
		int			fd;
		int			statuc_code;
		bool		end_request;
		std::string	buffer;
		std::string	outbuf;
		
		~Client();
		Client(int);
		Client(const Client &);
};