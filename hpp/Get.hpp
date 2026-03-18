#ifndef GET_HPP
#define GET_HPP

#include "Response.hpp"
#include "Client.hpp"
#include "Server.hpp"

class Get : virtual public Response
{
	private:
		Get();
		int		create_response();
	public:
		~Get();
		Get(Server &, Client &);
};

#endif