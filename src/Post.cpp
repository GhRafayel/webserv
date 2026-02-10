#include "../hpp/Post.hpp"

Post::~Post() {}

Post::Post(Server & s_obj, Client & c_obj) : Response(s_obj, c_obj) { 
	create_response() ;
}

bool Post::check_size()
{
	int size = client_ref.request.find("body")->second.size();

	if (client_ref.best_location_index != -1)
	{
		if (server_ref._locations[client_ref.best_location_index]._max_body_size < size || server_ref._body_max_size < size)
			return false;
	}
	return true;
}

void	Post::create_response()
{	
	path = abs_Path(client_ref.best_mach);
	size_t		post = path.rfind(".");
	if (post != std::string::npos)
		ext = path.substr(post, path.size());

	std::map<std::string, std::string>::iterator it = client_ref.request.find("body");
	
	if (!is_method_allowed())
		create_header(" 405 Not Allowed", false);
	else if (!exists(path,'\0'))
		create_header(" 404 Not Found", false);
	else if (check_size())
		create_header(" 423 Payload Too Large", false);
	else
	{
		body = it->second;
		std::ofstream file(abs_Path(client_ref.best_mach).c_str());
		if (!writable(path))
			create_header(" 500 Internal Server Error", true);
		file << body;
		create_header(" 200 ok", false);
	}
}
