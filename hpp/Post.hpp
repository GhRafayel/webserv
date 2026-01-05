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
		void	callFunctionByStatusCode();
		void	inishialize_fun_map();
		void	create_response();
		void	fun_201();
		void	fun_405();
		void	fun_404();
		void	fun_413();
		void	fun_500();
	public:
		~Post();
		Post(Server &, Client &);

};

#endif 