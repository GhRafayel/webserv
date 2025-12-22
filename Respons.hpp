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
		std::map<int, void (Respons::*) (void)>	fun_map;
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
		~Respons();
		Respons(Server &, Client &);
		Respons(const Respons &);
		Respons & operator=(const Respons &);
		void	send_respons();
};

#endif