
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
#include "Client.hpp"
#include "Server.hpp"
#include <fcntl.h>
#include <cstdio>
#include <cctype>

class CgiHandler :  virtual public Response
{
	private:
		std::string							_method;
		std::map<std::string, std::string>	_envMap;
		std::vector<std::string>			_envVec;
		std::string 						_output;
		void create_response();

		CgiHandler(const CgiHandler&);
		CgiHandler& operator=(const CgiHandler&);
		
		void createEnvironment();
		int execute();
		void setEnvVar(std::string, std::string);
		std::string getEnvVar(std::string);
		void convertEnv();
		std::string unchunkReq(std::string);
		std::string decodeQm(const std::string&);

	public:		
		CgiHandler( Server & s_obj,  Client & obj);
		~CgiHandler();

};

#endif
