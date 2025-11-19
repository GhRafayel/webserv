#ifndef MY_SERVER_HPP
#define MY_SERVER_HPP

#include "Client.hpp"
#include "Respons.hpp"
#include "Request.hpp"
#include "Pars.hpp"

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
		Request					_req;
		Respons					_res;
		struct  sockaddr_in		_s_addr;
		std::vector<client>		_client;
		std::vector<pollfd>		_fds;
		int 					_timeout_ms;
		int						_port;
		int						_socket;
	protected:	
		int			request(int);
		int			respons(int);
		void		create_socket();
		void		remove_item(int);
		void		add_item(int);
		bool		accept_loop();
		pollfd		create_pollfd(int);
	public:
		void		start();
		void		setPort(const int);
		~My_server();
		My_server();
};

#endif
