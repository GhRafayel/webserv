#ifndef Config_HPP
#define Config_HPP

#include "StringUtils.hpp"
#include "Request.hpp"
#include "Client.hpp"
#include "Server.hpp"

#include <sys/socket.h>
#include <fcntl.h>
#include <csignal>
#include <pthread.h>
#include <unistd.h>
#include <poll.h>

extern volatile bool g_running;

class Config : public StringUtils
{
	private:
		std::map<int, Client>	_client;
		std::map<int, Server *>	_servers;
		std::vector<pollfd>		_pollfds;
		int						_time;
		std::string				_conf_file_path;

		void	to_connect(int);
		bool	is_server_socket(int);
		void	initConfig();
		void	create_server();
		void	accept_loop();
		pollfd	create_pollfd(int);
	public:
		~Config();
		Config();
		Config(const Config &);
		Config(const std::string &);
		Config & operator = (const Config &);
		void    start();
};

#endif