#ifndef SERVER_HPP
#define SERVER_HPP

//#include "Request.hpp"
//#include "Respons.hpp"
#include <sys/socket.h>
#include <fcntl.h>
#include <csignal>

#include "Config.hpp"
#include "StringUtils.hpp"

#include <pthread.h>
#include <unistd.h>
#include <poll.h>

extern volatile bool g_running;

class Server : public StringUtils
{
	private:
		std::string					_conf_file_path;
		std::vector<Config>			_conf;
		std::vector<pollfd>			_pollfds;
		int							_time;
		void						initConfig();
		void						create_server();
		void						accept_loop();

	public:
		~Server();
		Server();
		Server(const std::string &);
		Server(const Server &);
		Server & operator = (const Server &);
		void	call_member(const std::string & fun_name);
		void    start(); 
};

#endif