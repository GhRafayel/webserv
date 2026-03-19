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
			to_read_cgi();
		else
		{
			CgiHandler * CGI = new CgiHandler(server_ref, client_ref);
			CGI->cgi_run();
			delete CGI;
		}
		if (client_ref.status_code < 200 || client_ref.status_code > 600) return 0;
	}
	if (client_ref.status_code >= 200 && client_ref.status_code <= 600) return 0;
	if (!is_method_allowed())
		return (client_ref.status_code = 405, 0);
	if (check_size())
		return (client_ref.status_code = 423, 0);
	size_t post = client_ref.best_match.rfind("/");
	std::ofstream file(abs_Path(client_ref.best_match.substr(0, post)) + client_ref.best_match.substr(post).c_str());
	if (!exists(abs_Path(client_ref.best_match)))
		return (client_ref.status_code = 404, 0);
	if (!writable(abs_Path(client_ref.best_match))) 
		return (client_ref.status_code = 500, 0);
	file << client_ref.request["body"];
	file.close();
	return (client_ref.status_code = 200, 0);
}
