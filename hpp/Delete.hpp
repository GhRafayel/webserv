#ifndef Delete_HPP
#define Delete_HPP

#include "Response.hpp"
#include "Client.hpp"
#include "Server.hpp"

class Delete : virtual public Response
{
	private:
		std::string								ext;
		std::string								body;
		std::map<int, void (Delete ::*) (void)>	fun_map;
		Delete ();
		Delete  &	operator = (const Delete  &);
		void		callFunctionByStatusCode(unsigned int);
		void		create_response();
	public:
		~Delete ();
		Delete (Server &, Client &);
};

#endif 