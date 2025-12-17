#ifndef RESPONS_HPP
#define RESPONS_HPP

#include "StringUtils.hpp"
#include "Client.hpp"
#include "Server.hpp"
#include <iostream>
#include <map>

class Respons : public StringUtils
{
	private:
		Respons();
		
		std::map<int, void (Respons::*) (void)>	fun_map;
		Client &								client_ref;
		Server &								server_ref;
		std::ostringstream						strim;
		std::string 							ext;
		std::string								body;
		void	fun_404();
		void	fun_200();
		void	fun_301();

		void	init_fun_map();
		void	callFunctionByStatusCode(unsigned int);
	public:
		~Respons();
		Respons( Server &,  Client &);
		void	send_respons();
};

#endif