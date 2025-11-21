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
		std::vector<std::string> methods;
		std::string default_file;
		std::string path;
		int         port;
		int			body_size;
		bool		seting;
		bool		location;
	public:
		Config(const std::string &);
		Config & operator = (const Config &);
		~Config();
		Config();

		int			getPort();
		void		read_conf_file();
		void		setSetings(std::vector<std::string> &);
		void		setPort(std::string &);
		void		setSize(std::string &);
		void		setPath(std::string &);
		void		setFileName(std::string &);
		void		setMethods(std::string &);
		void		check_otehr(const std::string &, std::string &);
		std::string abs_Path(const std::string &);
		std::string	getPath();
		std::string getDefaultFile();
		bool		getMethods(const std::string &);


};

#endif