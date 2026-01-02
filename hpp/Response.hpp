#ifndef Response_HPP
#define Response_HPP

#include "StringUtils.hpp"
#include "Client.hpp"
#include "Server.hpp"
#include <iostream>
#include <map>

class Response : public StringUtils
{
	protected:
		Server &			server_ref;
		Client &			client_ref;
		std::ostringstream	strim;
		std::string			status_code;

		bool				is_method_allowed();
		virtual void		create_response() = 0;
		virtual void		callFunctionByStatusCode(unsigned int) = 0;
		Response(Server &, Client &);
		Response(const Response &);
		Response &		operator=(const Response &);
	public:
		virtual ~Response();
		void			send_response();
};

#endif