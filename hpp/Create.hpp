#ifndef Create_HPP
#define Create_HPP

#include "Response.hpp"
#include "Client.hpp"
#include "Server.hpp"

class Create : virtual public Response
{
	private:
		std::string								ext;
		std::string								body;
		std::map<int, void (Create::*) (void)>	fun_map;
		Create();
		Create &	operator = (const Create &);
		void		callFunctionByStatusCode();
		void		create_response();
		void		inishialize_fun_map();
		void		fun_200();
		void		fun_405();
		void		fun_413();
		void		fun_500();
	public:
		~Create();
		Create(Server &, Client &);
};

#endif 