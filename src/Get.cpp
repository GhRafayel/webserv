#include "../hpp/Get.hpp"

Get::~Get() {}

Get::Get(Server & s_obj, Client & c_obj) : Response(s_obj, c_obj)
{
	create_response();
}

void	Get::create_response() {
	
	if (client_ref.is_cgi)
	{
		if (client_ref.statuc_code >= 200 && client_ref.statuc_code <= 600) return;
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
		if (client_ref.statuc_code < 200 || client_ref.statuc_code > 600) return;
	}
	if (client_ref.statuc_code >= 200 && client_ref.statuc_code <= 600) return;
	path = abs_Path(client_ref.best_mach);
	size_t		post = client_ref.best_mach.rfind(".");

	if (post != std::string::npos)
		ext = client_ref.best_mach.substr(post);

	if (!is_method_allowed()) 
		client_ref.statuc_code = 405;
	else if (path.empty()) 
		client_ref.statuc_code = 404;
	else if (!readable(path) || (is_directory(path) && !server_ref._locations[client_ref.best_location_index]._autoIndex))
		client_ref.statuc_code = 403;
	else if (client_ref.is_dir && is_directory(path))
	{
			if (exists(path + "/index.html"))
			{
				client_ref.best_mach += "/index.html";
				client_ref.statuc_code = 200;
			}
			else client_ref.statuc_code = 200200;
	}
	else client_ref.statuc_code = 200;
}

/*
void Get::fun_206()
{
	std::vector<std::string> temp = Range_pars(client_ref.buffer);
	if (!readable(path)) client_ref.statuc_code = 403;
	
	if (temp.empty()) client_ref.statuc_code = 400;
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
		client_ref.statuc_code = 200;
	}
}
*/

