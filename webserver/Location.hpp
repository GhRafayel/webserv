#ifndef LOCATION_HPP
#define LOCATION_HPP

#include "StringUtils.hpp"
#include <iostream>
#include <vector>
#include <map>

class Location : public StringUtils
{
	private:
		Location();
		int		count;
		void	control();
		void	init(std::vector<std::string> &, int i);
		void	location(std::string & );
		void	redirect(std::string & );
		void	root(std::string & );
		void	index(std::string & ) ;
		void	autoindex(std::string & );
		void	methods(std::string & );
		void	call_member(const std::string & , const char * );

	public:
		bool								valid;
		std::map<std::string, std::string>	args;
		std::map<std::string, bool>			allowed_methods;

		~Location();
		Location(std::vector<std::string> &, int );
		Location(const Location &);
		Location & operator = (const Location &);
};

#endif
