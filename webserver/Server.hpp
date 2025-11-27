#ifndef Server_HPP
#define Server_HPP

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>

#include "StringUtils.hpp"
#include "Location.hpp"
#include <typeinfo>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>

class Server : public StringUtils
{
	private:
		std::string	file_content;
		bool		valid;

		void		listen(std::string & str);
		void		client_max_body_size(std::string & str);
		void		server_name(std::string & str);
		void		error_page_404(std::string & str);
		void		error_page_500(std::string & str);
		void		control();
		void		call_member(const std::string &,const  char *);
		void		read_conf();

	public:
		struct  sockaddr_in			addr;
		std::vector<Location>		conf_location;
		std::map<std::string, int>	conf_seting;
		std::string					serv_name;
		std::string					error_404;
		std::string					error_500;
		
		~Server();
		Server();
		Server(std::string &);
		Server(const Server &);
		Server & operator = (const Server &);
		
		bool		is_valid() const;
};

#endif

