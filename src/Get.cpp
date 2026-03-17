#include "../hpp/Get.hpp"

Get::~Get() {}

Get::Get(Server & s_obj, Client & c_obj) : Response(s_obj, c_obj)
{
	create_response();
}

int	Get::create_response() {
	
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

	if (!is_method_allowed()) 
		return (client_ref.status_code = 405, 0);
	if (path.empty()) 
		return (client_ref.status_code = 404, 0);
	if (!readable(path) || (is_directory(path) && !server_ref._locations[client_ref.best_location_index]._autoIndex))
		return (client_ref.status_code = 403, 0);
	if (client_ref.is_dir && is_directory(path))
	{
			if (exists(path + "/index.html"))
			{
				client_ref.best_match += "/index.html";
				return (client_ref.status_code = 200, 0);
			}
			return (client_ref.status_code = 200200, 0);
	}
	return (client_ref.status_code = 200, 0);
}

/*
void Get::fun_206()
{
	std::vector<std::string> temp = range_parse(client_ref.buffer);
	if (!readable(path)) client_ref.status_code = 403;
	
	if (temp.empty()) client_ref.status_code = 400;
	else
	{
		int	start = str_to_int(temp[0]);
		int	end = (temp[1].empty() ? -1 : str_to_int(temp[1]));
		if (end == -1)
			body = get_file_content(path);
		else
			body = get_file_content(path, start, end - start + 1);
		std::stringstream s;
		s << " 200 Partial Content\r\n Content-Range: bytes " << start << "-" << end << "/" << body.size();
		client_ref.status_code = 200;
	}
}
*/

