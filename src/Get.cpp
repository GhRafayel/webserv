#include "../hpp/Get.hpp"

Get::~Get() {}

Get::Get(Server & s_obj, Client & c_obj) : Response(s_obj, c_obj)
{
	if (client_ref.statuc_code == 301)
		create_header(" Moved Permanently", false);
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
	{
		client_ref.statuc_code = 405;
		create_header(" Not Allowed", false);
	}
	else if (path.empty())
	{
		client_ref.statuc_code = 404;
		client_ref.best_mach = server_ref._error_404;
		create_header(" Not Found", true);
	}
	else if (is_directory(path))
	{
		if (client_ref.best_location_index == -1 || !server_ref._locations[client_ref.best_location_index]._autoIndex)
		{
			client_ref.statuc_code = 403;
			create_header(" Forbidden", true);
		}
		else
		{
			client_ref.statuc_code = 200;
			client_ref.is_dir = true;
			create_header(" ok", true);
		}
	}
	else
	{
		client_ref.statuc_code = 200;
		create_header(" ok", true);
	}
}

void Get::fun_206()
{
	if (!readable(path))
		create_header(" Forbidden", false);
	std::vector<std::string> temp = Range_pars(client_ref.buffer);
	if (temp.empty()) 
		create_header(" Bed Request", false);
	else
	{
		int	start = str_to_int(temp[0]);
		int	end = (temp[1].empty() ? -1 : str_to_int(temp[1]));
		if (end == -1)
			body = get_file_content(path);
		else
			body = get_file_content(path, start, end - start + 1);
		std::stringstream s;
		s << " Partial Content\r\n Content-Range: bytes " << start << "-" << end << "/" << body.size();
		create_header(s.str(), true);
	}
}
