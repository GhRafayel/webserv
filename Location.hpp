#ifndef LOCATION_HPP
#define LOCATION_HPP

#include "StringUtils.hpp"
#include <iostream>
#include <vector>
#include <map>

class Location : public StringUtils
{
	private:
		void	init();
		void	location(std::string &);
		void	redirect(std::string &);
		void	root(std::string &);
		void	index(std::string &) ;
		void	autoindex(std::string &);
		void	methods(std::string &);
		bool	is_line_valid(std::string &);
		std::vector<std::string>	_config;
	public:
		bool		_GET;
		bool		_POST;
		bool		_DELETE;
		bool		_autoIndex;
		std::string	_root;
		std::string	_redirection;
		std::string	_index;
		std::string	_location;
		std::string _error_massage;

		~Location();
		Location();
		Location(std::vector<std::string> &);
		Location(const Location &);
		Location & operator = (const Location &);
		void	callFunctionByName(const std::string &, std::string &);
};

#endif

