#ifndef GET_HPP
#define GET_HPP

#include "Response.hpp"
#include "Client.hpp"
#include "Server.hpp"

class Get : virtual public Response
{
	private:
		Get();
		void	create_response();
		void	fun_206();
	public:
		~Get();
		Get(Server &, Client &);
};

#endif