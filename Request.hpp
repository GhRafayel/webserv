#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "StringUtils.hpp"
#include <sys/socket.h>
#include "Client.hpp"
#include "Server.hpp"
#include <iostream>
#include <map>


class Request : public StringUtils
{
	private:
		Server								&server_ref;
		Client								&client_ref;
		std::string							method;
		std::string							path;
		std::string							protocol;
		std::map<std::string, std::string>	request;
		int									best_location_index;

	public:
		void								analize_request();
		std::string							getMethod();
		std::string							getPath();
		std::string 						getProtocol();
		std::map<std::string, std::string>	&getRequest();
		std::string							get_best_mach(const std::string &);
		std::string							is_defoult_location(const std::string &);
		bool								is_directory(const std::string &);



		~Request();
		//Request();
		Request(Server &, Client &);
		Request(const Request &);
		Request &	operator = (const Request &);
		void	run();
};

#endif