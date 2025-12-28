#ifndef Response_HPP
#define Response_HPP

#include "StringUtils.hpp"
#include "Client.hpp"
#include "Server.hpp"
#include <iostream>
#include <map>

class Response : public StringUtils
{
	private:
		std::map<int, void (Response::*) (void)>	fun_map;
		Server &								server_ref;
		Client &								client_ref;
		std::ostringstream						strim;
		std::string								status_code;
		std::string 							ext;
		std::string								body;
		void	fun_200();
		void	fun_206();
		void	fun_301();
		void	fun_400();
		void	fun_403();
		void	fun_404();
		void	fun_405();
		void	init_fun_map();
		void	callFunctionByStatusCode(unsigned int);
	public:
		~Response();
		Response(Server &, Client &);
		Response(const Response &);
		Response & operator=(const Response &);
		void	send_response();
};

#endif