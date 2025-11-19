#ifndef MY_SERVER_HPP
#define MY_SERVER_HPP

#include "Client.hpp"

#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sstream>
#include <fstream>
#include <cerrno>
#include <vector>
#include <csignal>

extern volatile bool g_running;

class My_server
{
	private:
		struct  sockaddr_in		_s_addr;
		std::vector<client>		_client;
		std::vector<pollfd>		_pollfds;
		int 					_timeout_ms;
		int						_port;
		int						_s_socket;
	protected:	
		int			request(int);
		int			respons(int);
		void		init();
		bool		create_socket();
		bool		accept_loop();
		pollfd		create_pollfd(int);
	public:
		void			start();
		void			setPort(const int);
		~My_server();
		My_server();
};

#endif
