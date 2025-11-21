#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include "Pars.hpp"

class Config
{
	private:
		std::string buffer;
		std::string path;
		int         port;
	public:
		Config(const std::string &);
		Config & operator = (const Config &);
		~Config();
		Config();

		int			getPort();
		void		check_others(std::vector<std::string> &);
		void		read_conf_file();
		void		setPort(std::string &);
		std::string abs_Path(const std::string &);
		std::string	getPath();
};

#endif