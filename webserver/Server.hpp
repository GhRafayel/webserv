#ifndef SERVER_HPP
#define SERVER_HPP

#include "StringUtils.hpp"
#include "Request.hpp"
#include "Client.hpp"
#include "Config.hpp"

#include <sys/socket.h>
#include <fcntl.h>
#include <csignal>
#include <pthread.h>
#include <unistd.h>
#include <poll.h>

extern volatile bool g_running;

class Server : public StringUtils
{
	private:
		std::map<int, Client>			_client;
		std::map<int, Config *>			_conf;
		std::vector<pollfd>				_pollfds;
		int								_time;
		std::string						_conf_file_path;

		Server(const Server &);

		void	to_connect(int);
		bool	is_server_socket(int);
		void	initConfig();
		void	create_server();
		void	accept_loop();
		pollfd	create_pollfd(int);
	public:
		~Server();
		Server();
		Server(const std::string &);
		Server & operator = (const Server &);
		void    start();
};

#endif