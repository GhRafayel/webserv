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

		void		call_member(const std::string &);
		void		read_conf();
		void		init_sockadd_struct();
	public:
		struct  sockaddr_in			addr;
		std::vector<Location>		conf_location;
		std::map<std::string, int>	conf_seting;
		std::string					error_404;
		std::string 				error_500;
		
		~Config();
		Config();
		Config(std::string &);
		Config(const Config &);
		Config & operator = (const Config &);
		
		bool		is_valid() const;
		

		// void		setSetings(std::vector<std::string> &);

		// void		setPort(std::string &);
		// void		setSize(std::string &);
		// void		setPath(std::string &);
		// void		setFileName(std::string &);
		// void		setMethods(std::string &);
		
		// void		check_otehr(const std::string &, std::string &);
		// std::string abs_Path(const std::string &);
		// int			getPort();
		// std::string	getPath();
		// std::string getDefaultFile();
		// bool		getMethods(const std::string &);
};

#endif

