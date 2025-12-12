#ifndef MY_SERVER_HPP
#define MY_SERVER_HPP

#include "StringUtils.hpp"
#include "ConfigPars.hpp"
#include "Request.hpp"
#include "Client.hpp"
#include "Server.hpp"

#include <sys/socket.h>
#include <fcntl.h>
#include <csignal>
#include <pthread.h>
#include <unistd.h>
#include <poll.h>
#include <algorithm>

extern volatile bool g_running;

class My_server : public StringUtils
{
	private:
		std::map<int, Client>		_client;
		std::map<int, Server>		_servers;
		std::vector<pollfd>			_pollfds;
		int							_time;
		std::string					_conf_file_path;
		std::vector<std::string>	_env;

		void	to_connect(int);
		bool	is_server_socket(int);
		void	initConfig();
		void	create_server(const std::map<int, Server>::iterator &);
		void	accept_loop();
		void	remove_conection(int);
		int		to_read(Client &);
		pollfd	create_pollfd(int);
	public:
		~My_server();
		My_server();
		My_server(const My_server &);
		My_server(char **env);
		My_server(const std::string &,  char **env);
		My_server & operator = (const My_server &);
		void    start_server();
		void	poll_in(int);
		void	poll_out(int);
		void	time_out();
		
};

#endif