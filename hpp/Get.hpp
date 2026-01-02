#ifndef GET_HPP
#define GET_HPP

#include "Response.hpp"
#include "Client.hpp"
#include "Server.hpp"

class Get : virtual public Response
{
	private:
		std::string							ext;
		std::string							body;
		std::map<int, void (Get::*) (void)>	fun_map;
		Get();
		Get &	operator = (const Get &);
		void	callFunctionByStatusCode(unsigned int);
		void	create_response();
		void	fun_200();
		void	fun_206();
		void	fun_301();
		void	fun_400();
		//void	fun_401(); // Unauthorized
		void	fun_403();
		void	fun_404();
		void	fun_405();

		// void	fun_411(); // Length Required (only if you demand Content-Length on GET with body)
		// void	fun_413(); // URI Too Long
		// void	fun_414(); // URI Too Long
		// void	fun_500(); // Internal Server Error
		// void	fun_503(); // Service Unavailable
	public:
		~Get();
		Get(Server &, Client &);
};

#endif