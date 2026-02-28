#ifndef LOCATION_HPP
#define LOCATION_HPP

#include "StringUtils.hpp"
#include <iostream>
#include <vector>
#include <map>

class Location : public StringUtils
{
	private:
		std::map<std::string, void (Location::*)(std::string &)> func_map;

		void	init();
		void	location_pars();
		void	cgi_extension(std::string &);
		void	loc_location(std::string &);
		void	loc_return(std::string &);
		void	loc_root(std::string &);
		void	loc_index(std::string &) ;
		void	loc_autoindex(std::string &);
		void	loc_methods(std::string &);
		bool	is_line_valid(std::string &);
		void	loc_max_body_size(std::string &);


		std::vector<std::string>	_config;
		std::map<std::string, bool> _methods;
	public:
		bool						_autoIndex;
		std::string					_root;
		std::string					_index;
		std::string					_location;
		std::string					_error_massage;
		std::vector<std::string>	_return;
		std::vector<std::string>	_cgi;
		int							_max_body_size;

		~Location();
		Location();
		Location(std::vector<std::string> &);
		Location(const Location &);
		Location & operator = (const Location &);
		void	callFunctionByName(const std::string &, std::string &);
		bool	get_method(const std::string &);
};

#endif

