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
		void		init_fun_map();
	public:
		~Respons();
		Respons( Server &,  Client &);
		void	fun_404();
		void	fun_200();

		void	send_respons();
		void	callFunctionByStatusCode(unsigned int);
};

#endif