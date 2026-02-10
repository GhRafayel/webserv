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
		std::string			body;
		std::string			ext;
		bool				is_method_allowed();
		void				create_header(const std::string & msg, bool);
		virtual void		create_response() = 0;
		Response &			operator=(const Response &);
		Response(Server &, Client &);
		Response(const Response &);
		
	public:
		virtual		~Response();
		void		send_response();
};

#endif