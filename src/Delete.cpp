#include "../hpp/Delete.hpp"

Delete::~Delete () {}

Delete::Delete (Server & s_obj, Client & c_obj) : Response(s_obj, c_obj)
{
	create_response();
}

void	Delete::create_response()
{
	path = abs_Path(client_ref.best_mach);
	size_t	post = client_ref.best_mach.rfind(".");

	if (post != std::string::npos)
		ext = client_ref.best_mach.substr(post, client_ref.best_mach.size());

	if (!is_method_allowed())
	{
		client_ref.statuc_code = 405;
		create_header(" Forbidden", false);
	}
	else
	{
		if (!exists(path, '\0'))
		{
			client_ref.statuc_code = 404;
			create_header(" Not Found", false);
		}
		else if (!writable(path) || remove(path.c_str()) != 0)
		{
			client_ref.statuc_code = 500;
			create_header("Internal Server Error", false);
		}
		else
		{
			client_ref.statuc_code = 200;
			create_header(" ok", false);
		}
	}
}