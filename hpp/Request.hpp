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
		Request();
		Server		&server_ref;
		Client		&client_ref;
		std::string	url_path;
		std::string	protocol;
		int			best_location_index;

		void	pars_request();
		void	get_best_mach();
		bool	is_defoult_location();
		bool	is_method_allowed();
		void	start_request();

	public:
		~Request();
		Request(Server &, Client &);
		Request(const Request &);
		Request &	operator = (const Request &);
};

#endif