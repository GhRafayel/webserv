#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "StringUtils.hpp"
#include <sys/socket.h>
#include <dirent.h>
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
		std::string							url_path;
		std::string							protocol;
		std::map<std::string, std::string>	request;
		int									best_location_index;

	public:
		void	analize_request();
		void	get_best_mach();
		bool	is_defoult_location();
		bool	is_method_allowed();
		bool	is_directory();

		~Request();
		//Request();
		Request(Server &, Client &);
		Request(const Request &);
		Request &	operator = (const Request &);
		void		run();
};

#endif