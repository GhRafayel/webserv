#ifndef STRINGUTILS_HPP
#define STRINGUTILS_HPP

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

class StringUtils 
{
	private:
		StringUtils(const StringUtils &);
		StringUtils & operator = (const StringUtils &);
	public:
		~StringUtils();
		StringUtils();
		void						validate_file(const std::string &);
		bool						is_digitS(const std::string &);
		bool						recieve(const std::string &);
		int							str_to_int(std::string &);
		std::string                 trim(std::string &, const std::string &);
		std::string					abs_Path(const std::string &);
		std::string					chang_char(std::string &, const char, const char);
		std::vector<std::string>	split(std::string &, const std::string &, bool);

};


#endif