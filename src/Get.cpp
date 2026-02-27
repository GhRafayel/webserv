#include "../hpp/Get.hpp"

Get::~Get() {}

Get::Get(Server & s_obj, Client & c_obj) : Response(s_obj, c_obj)
{
	if (client_ref.statuc_code == 301)
		create_header(" 301 Moved Permanently", false);
	else if (client_ref.statuc_code == 206)
		fun_206();
	else
		create_response();
}

void	Get::create_response() {

	path = abs_Path(client_ref.best_mach);
	size_t		post = client_ref.best_mach.rfind(".");

	if (post != std::string::npos)
		ext = client_ref.best_mach.substr(post);

	if (!is_method_allowed())
		create_header(" 405 Not Allowed", false);
	else if (path.empty())
	{
		path = abs_Path( server_ref._error_404);
		ext = ".html";
		create_header(" 404 Not Found", true);
	}
	else if (!readable(path))
		create_header(" 403 Forbidden", false);
	else if (is_directory(path))
	{
		if (!server_ref._locations[client_ref.best_location_index]._autoIndex)
			create_header(" 403 Forbidden", false);
		else 
		{
			if (exists(path + "index.html"))
			{
				path += "index.html";
				create_header(" 200 ok", true);
			}
			else
			{
				ext = ".html";
				create_header(" 200 ok", false);
				client_ref.outbuf += static_page();
			}
		}
	}
	else
		create_header(" 200 ok", true);
}

void Get::fun_206()
{
	if (!readable(path))
		create_header(" 403 Forbidden", false);
	std::vector<std::string> temp = Range_pars(client_ref.buffer);
	if (temp.empty()) 
		create_header(" 400 Bed Request", false);
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
		create_header(s.str(), true);
	}
}


