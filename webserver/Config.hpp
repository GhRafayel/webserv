#ifndef CONFIG_HPP
#define CONFIG_HPP

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

class Config : public StringUtils
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
		
		~Config();
		Config();
		Config(std::string &);
		Config(const Config &);
		Config & operator = (const Config &);
		
		bool		is_valid() const;
};

#endif

