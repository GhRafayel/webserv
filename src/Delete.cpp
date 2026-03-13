#include "../hpp/Delete.hpp"

Delete::~Delete () {}

Delete::Delete (Server & s_obj, Client & c_obj) : Response(s_obj, c_obj)
{
	create_response();
}

void	Delete::create_response()
{
	path = abs_Path(client_ref.best_match);
	size_t	post = client_ref.best_match.rfind(".");

	if (post != std::string::npos)
		ext = client_ref.best_match.substr(post, client_ref.best_match.size());

	if (!is_method_allowed()) client_ref.status_code = 405;
	else
	{
		if (!exists(path)) client_ref.status_code = 404;
		else if (remove(path.c_str()) != 0) client_ref.status_code = 500;
		else client_ref.status_code = 200;
	}
}