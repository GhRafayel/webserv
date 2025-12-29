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
		Server &									server_ref;
		Client &									client_ref;
		std::ostringstream							strim;
		std::string									status_code;
	public:

		virtual ~Response();
		Response(Server &, Client &);
		Response(const Response &);
		Response &		operator=(const Response &);
		void			send_response();
		virtual void	callFunctionByStatusCode(unsigned int) = 0;
};

#endif