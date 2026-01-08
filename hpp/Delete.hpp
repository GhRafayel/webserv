#ifndef Delete_HPP
#define Delete_HPP

#include "Response.hpp"
#include "Client.hpp"
#include "Server.hpp"

class Delete : virtual public Response
{
	private:
		Delete ();
		void		create_response();
	public:
		~Delete ();
		Delete (Server &, Client &);
};

#endif 