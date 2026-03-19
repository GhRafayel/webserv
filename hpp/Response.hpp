#ifndef Response_HPP
#define Response_HPP

#include "CgiHandler.hpp"
#include "StringUtils.hpp"
#include "Client.hpp"
#include "Server.hpp"

class Response : public StringUtils
{
	private:
		std::map<int, void (Response::*) (void)>	func_map;
		std::string									end_line;
		void										init();
		void										to_send();
		void										fun_200200();
		void										fun_200();
		void										fun_206();
		void										fun_301();
		void										fun_400();
		void										fun_403();
		void										fun_404();
		void										fun_405();
		void										fun_408();
		void										fun_423();
		void										fun_500();
		void										fun_504();
		void										cgi_outPut_pars();
		std::string									static_page();
	protected:
		Server &									server_ref;
		Client &									client_ref;
		std::ostringstream							strim;
		std::string									body;
		bool										is_method_allowed();
		void										create_header();
		void										to_read_cgi();
		virtual int									create_response() = 0;
		Response &									operator=(const Response &);
		Response(Server &, Client &);
		Response(const Response &);
	public:
		virtual		~Response();
		void		send_response();
};

#endif