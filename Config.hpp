#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include "Pars.hpp"

class Config {
	private:
		std::string buffer;
		std::string path;
		int         port;
	public:
	Config(const std::string &);
	~Config();
	void	setPort(const int);
	void	setPath(const std::string new_path);
	int		getPort();
	void	read_conf_file();
	void	setPort(std::string &);
    void	setPath(std::string &);
};

#endif