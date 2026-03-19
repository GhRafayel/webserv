#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include "Response.hpp"
#include "StringUtils.hpp"
#include "Client.hpp"
#include "Server.hpp"

class CgiHandler : public StringUtils
{
	private:
		std::map<std::string, std::string>	_envMap;
		std::vector<std::string>			_envVec;
		Server								&server_ref;
		Client								&client_ref;		

		std::string interp;
		std::string script;

		CgiHandler(const CgiHandler&);
		std::string dirname_from_path(const std::string &path);
		std::string	basename_from_path(const std::string &path);

		CgiHandler& 		operator=(const CgiHandler&);
		std::string 		getEnvVar(std::string &);
		std::string 		unchunkReq(std::string &);
		std::string 		decodeQm(const std::string&);
		void				convertEnv();
		void				createEnvironment();
		void				setEnvVar(const std::string &, const std::string &);
		bool				is_method_allowed();
		int					execute();
		std::vector<char*>	init_envp();
		void				child_process(std::vector<char*>&);

	public:		
		CgiHandler( Server & s_obj,  Client & obj);
		~CgiHandler();
		int		cgi_run();
};

#endif
