#ifndef STRINGUTILS_HPP
#define STRINGUTILS_HPP

#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctime>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>

class StringUtils 
{
	private:
		StringUtils(const StringUtils &);
		StringUtils & operator = (const StringUtils &);
		std::map<std::string, std::string>	_my_tayps;
	public:
		~StringUtils();
		StringUtils();
		void						init();
		void						validate_file(const std::string &);
		bool						is_digitS(const std::string &);
		bool						is_end_of_request(const std::string &);
		int							str_to_int(const std::string &);
		std::string                 trim(std::string &, const std::string &);
		std::string					trim (const std::string &, const std::string &);
		std::string					abs_Path(const std::string &);
		std::string					chang_char(std::string &, const char, const char);
		std::vector<std::string>	split(std::string &, const std::string &, bool);
		std::string					get_file_content(const std::string & );
		std::string					get_file_content(const std::string & , size_t , size_t);
		std::string 				int_to_string(int);
		std::string					get_my_taype(const std::string &);
		bool						is_directory(const std::string &);
		bool						readable(const std::string &);
		bool						writable(const std::string &);
		bool						executable(const std::string &);
		bool						exists(const std::string &);
		std::vector<std::string>	Range_pars(const std::string &);



		std::string 				get_http_date();
};

#endif