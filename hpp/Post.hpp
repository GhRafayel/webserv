#ifndef Post_HPP
#define Post_HPP

#include "Response.hpp"
#include "Client.hpp"
#include "Server.hpp"

class Post : virtual public Response
{
	private:
		Post();
		void	create_response();
		bool	check_size();
	public:
		~Post();
		Post(Server &, Client &);
};

#endif 