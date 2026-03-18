#include "../hpp/Delete.hpp"

Delete::~Delete () {}

Delete::Delete (Server & s_obj, Client & c_obj) : Response(s_obj, c_obj)
{
	create_response();
}

int	Delete::create_response()
{
	if (client_ref.status_code >= 200 && client_ref.status_code <= 600) return 0;

	path = abs_Path(client_ref.best_match);
	size_t	post = client_ref.best_match.rfind(".");

	if (post != std::string::npos)
		ext = client_ref.best_match.substr(post, client_ref.best_match.size());

	if (!is_method_allowed()) 
		return (client_ref.status_code = 405, 0);
	if (!exists(path)) 
		return (client_ref.status_code = 404, 0);
	if (remove(path.c_str()) != 0) 
		return (client_ref.status_code = 500, 0);
	return (client_ref.status_code = 200, 0);
}