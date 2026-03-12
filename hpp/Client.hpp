#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <iostream>
#include <sstream>
#include <ctime>
#include <map>

class Client
{		
	public:
		bool								cgi_run;
		pid_t								cgi_pid;
		int									out_pipe[2];
		int									in_pipe[2];
		std::time_t							timeOut;
		int									best_location_index;
		int 								server_conf_key;
		int									fd;
		int									statuc_code;
		bool								is_dir;
		bool								is_cgi;
		bool								end_request;
		std::string							query;
		std::string							buffer;
		std::string							outbuf;
		std::string							cgibuf;
		std::string							cgi_type;
		std::string							best_mach;
		std::string							method;
		std::map<std::string, std::string>	request;
		
		~Client();
		Client();
		Client(int);
		Client(const Client &);
		Client & operator = (const Client &);
};

#endif