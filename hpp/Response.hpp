#ifndef Response_HPP
#define Response_HPP

#include "../hpp/CgiHandler.hpp"
#include "StringUtils.hpp"
#include "Client.hpp"
#include "Server.hpp"
#include <iostream>
#include <map>

class Response : public StringUtils
{
	private:
		std::map<int, void (Response::*) (void)>	func_map;
		std::string									end_line;
		void										init();
	protected:
		Server &			server_ref;
		Client &			client_ref;
		std::ostringstream	strim;
		std::string			path;
		std::string			body;
		std::string			ext;
		std::string			static_page();
		bool				is_method_allowed();
		void				create_header();
		virtual void		create_response() = 0;
		Response &			operator=(const Response &);
	
		void				fun_200200();
		void				fun_200();
		void				fun_206();
		void				fun_301();
		void				fun_400();
		void				fun_403();
		void				fun_404();
		void				fun_405();
		void				fun_423();
		void				fun_500();
		
		Response(Server &, Client &);
		Response(const Response &);
		
	public:
		virtual		~Response();
		void		send_response();
};

#endif