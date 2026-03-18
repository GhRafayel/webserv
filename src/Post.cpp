#include "../hpp/Post.hpp"

Post::~Post() {}

Post::Post(Server & s_obj, Client & c_obj) : Response(s_obj, c_obj) { 
	create_response() ;
}

bool Post::check_size()
{
	int size = client_ref.request.find("body")->second.size();
	if (server_ref._locations[client_ref.best_location_index]._max_body_size > size || server_ref._body_max_size > size)
		return false;
	return true;
}

int	Post::create_response()
{	
	if (client_ref.is_cgi)
	{
		if (client_ref.status_code >= 200 && client_ref.status_code <= 600) return 0;
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
		if (client_ref.status_code < 200 || client_ref.status_code > 600) return 0;
	}
	if (client_ref.status_code >= 200 && client_ref.status_code <= 600) return 0;
	path = abs_Path(client_ref.best_match);
	size_t		post = client_ref.best_match.rfind(".");
	if (post != std::string::npos)
		ext = client_ref.best_match.substr(post);

	std::map<std::string, std::string>::iterator it = client_ref.request.find("body");
	
	if (!is_method_allowed())
		return (client_ref.status_code = 405, 0);
	if (check_size())
		return (client_ref.status_code = 423, 0);
	post =  client_ref.best_match.rfind("/");
	std::string new_path = abs_Path(client_ref.best_match.substr(0, post)) + client_ref.best_match.substr(post).c_str();
	body = it->second;
	std::ofstream file(new_path.c_str(), std::ios::binary);
	if (!writable(new_path)) 
		return (client_ref.status_code = 500, 0);
	file << body;
	file.close();
	return (client_ref.status_code = 200, 0);
}
