#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "StringUtils.hpp"
#include "Client.hpp"
#include "Server.hpp"

class Request : public StringUtils
{
	private:
		Request();
		Server		&server_ref;
		Client		&client_ref;
		
		bool		parse_request();
		void		upload_parser(std::string &);
		void		get_best_match(std::string &);
		void		find_cgi();
		
	public:
		~Request();
		Request(Server &, Client &);
		Request(const Request &);
		Request &	operator = (const Request &);
};

#endif