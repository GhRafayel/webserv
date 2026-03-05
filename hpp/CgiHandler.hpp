
#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP


#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <unistd.h>
#include <cstdlib>
#include <sys/wait.h>
#include "Response.hpp"
#include "StringUtils.hpp"
#include "Client.hpp"
#include "Server.hpp"
#include <fcntl.h>
#include <cstdio>
#include <cctype>

class CgiHandler : public StringUtils
{
	private:
		std::string							_method;
		std::map<std::string, std::string>	_envMap;
		std::vector<std::string>			_envVec;
		std::string 						_output;
		Server								&server_ref;
		Client								&client_ref;				
		

		CgiHandler(const CgiHandler&);
		CgiHandler& operator=(const CgiHandler&);
		std::string	cgi_type();
		std::string getEnvVar(std::string);
		std::string unchunkReq(std::string);
		std::string decodeQm(const std::string&);
		void		convertEnv();
		void		createEnvironment();
		void		setEnvVar(std::string, std::string);
		bool		is_method_allowed();
		bool		cgi_exist();
		int			execute();

	public:		
		CgiHandler( Server & s_obj,  Client & obj);
		~CgiHandler();
		void		cgi_run();


};

#endif
