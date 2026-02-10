
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
		
	public:		
		CgiHandler( Server & s_obj,  Client & obj);

		~CgiHandler();

		int execute();
		

		void setMethod(std::string);
		void setEnvVar(std::string, std::string);
		void convertEnv();


		const std::string& getOutput() const;

		// void setHeaderEnv(std::map<std::string, std::string>&);
};

#endif
