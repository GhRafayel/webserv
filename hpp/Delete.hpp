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
		void		callFunctionByStatusCode();
		void		inishialize_fun_map();
		void		create_response();
		void		fun_200();
		void		fun_403();
		void		fun_404();
		void		fun_500();
	public:
		~Delete ();
		Delete (Server &, Client &);
};

#endif 