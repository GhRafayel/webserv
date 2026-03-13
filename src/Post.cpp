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
		if (server_ref._locations[client_ref.best_location_index]._max_body_size > size || server_ref._body_max_size > size)
			return false;
	}
	return true;
}

void	Post::create_response()
{	
	if (client_ref.is_cgi)
	{
		if (client_ref.status_code >= 200 && client_ref.status_code <= 600) return;
		if (client_ref.cgi_run)
		{
			to_read_cgi();
		}
		else
		{
			CgiHandler * CGI = new CgiHandler(server_ref, client_ref);
			CGI->cgi_run();
			delete CGI;
		}
		if (client_ref.status_code < 200 || client_ref.status_code > 600) return;
	}
	if (client_ref.status_code >= 200 && client_ref.status_code <= 600) return;
	path = abs_Path(client_ref.best_match);
	size_t		post = path.rfind(".");
	if (post != std::string::npos)
		ext = path.substr(post, path.size());

	std::map<std::string, std::string>::iterator it = client_ref.request.find("body");
	
	if (!is_method_allowed())
		client_ref.status_code = 405;
	else if (check_size())
		client_ref.status_code = 423;
	else
	{
		size_t n =  client_ref.best_match.rfind("/");
		std::string new_path = abs_Path(client_ref.best_match.substr(0, n)) + client_ref.best_match.substr(n).c_str();
		body = it->second;
		std::ofstream file(new_path.c_str());
		if (!writable(new_path)) 
			client_ref.status_code = 500;
		file << body;
		file.close();
		client_ref.status_code = 200;
	}
}
