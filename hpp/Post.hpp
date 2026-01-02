#ifndef Post_HPP
#define Post_HPP

#include "Response.hpp"
#include "Client.hpp"
#include "Server.hpp"

class Post : virtual public Response
{
	private:
		std::string								ext;
		std::string								body;
		std::map<int, void (Post::*) (void)>	fun_map;
		Post();
		Post &	operator = (const Post &);
		void	callFunctionByStatusCode(unsigned int);
		void	create_response();
		void	fun_200();
		void	fun_405();
		void	fun_404();

	public:
		~Post();
		Post(Server &, Client &);

};

#endif 