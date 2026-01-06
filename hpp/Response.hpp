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
		std::string			path;
		bool				is_method_allowed();
		virtual void		inishialize_fun_map() = 0;
		virtual void		create_response() = 0;
		virtual void		callFunctionByStatusCode() = 0;
		Response &			operator=(const Response &);
		Response(Server &, Client &);
		Response(const Response &);
		
	public:
		virtual ~Response();
		void		send_response();
};

#endif