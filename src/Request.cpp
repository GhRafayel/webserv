#include "../hpp/Request.hpp"

Request::~Request() {}

Request::Request(Server & S_obj, Client & C_obj) : StringUtils(),
	server_ref(S_obj),
	client_ref(C_obj),
	best_location_index(-1)
{
	pars_request();
	start_request();
}

Request::Request(const Request & obj) : StringUtils(),
	server_ref(obj.server_ref),
	client_ref(obj.client_ref)
{
	this->url_path = obj.url_path;
	this->protocol = obj.protocol;
	this->best_location_index = obj.best_location_index;
}

Request &	Request::operator = (const Request & obj)
{
	if (this != &obj)
	{
		this->server_ref = obj.server_ref;
		this->client_ref = obj.client_ref;
		this->url_path = obj.url_path;
		this->protocol = obj.protocol;
		this->best_location_index = obj.best_location_index;
	}
	return *this;
}

void	Request::pars_request()
{
	std::cout << client_ref.buffer << std::endl;

	if (client_ref.buffer.find("Range:") == 0)
	{
		client_ref.statuc_code = 206;
		client_ref.method = "GET";
		return ;
	}

	std::vector<std::string> req = split(client_ref.buffer, "\r\n", true);
	std::vector<std::string> header = split(req[0], " ", true);
	
	client_ref.method = header[0];
	this->url_path = header[1];
	this->protocol = header[2];

	for (size_t i = 1; i < req.size(); i++)
	{
		size_t post = req[i].find(":");
		
		std::string key = trim(req[i].substr(0, post), " ");

		std::string value = trim(req[i].substr(post, req[i].size()), " ");

		client_ref.request.insert(std::make_pair(key, value + "\r\n"));
	}
	
	client_ref.request.insert(std::make_pair("protocol", protocol));
}

bool	Request::is_defoult_location()
{
	if (url_path == "/")
	{
		client_ref.best_mach =  abs_Path(server_ref._root + server_ref._index);
		return true;
	}
	if (url_path.substr(1, url_path.size()).find("/") == std::string::npos)
	{
		client_ref.best_mach =  abs_Path(server_ref._root + url_path);
		return true;
	}
	return false;
}

void Request::get_best_mach()
{
	int				best_index = -1;
	std::string		best_loc;
	
	if (is_defoult_location()) return;
	
	for (size_t i = 0; i < server_ref._locations.size(); i++)
	{
		std::string loc = server_ref._locations[i]._location;
		if (url_path.find(loc) == 0 && loc.size() > best_loc.size())
		{
			best_loc = loc;
			best_index = i;
		}
	}
	if (best_index != -1)
	{
		if (!server_ref._locations[best_index]._return.empty())
		{
			client_ref.statuc_code = str_to_int(server_ref._locations[best_index]._return[0]);
			client_ref.best_mach = server_ref._locations[best_index]._return[1];
			return ;
		}
		std::string relative_path = url_path.substr(best_loc.size());
		std::string real_path = server_ref._locations[best_index]._root + relative_path;

		if (!server_ref._locations[best_index]._index.empty() && relative_path.empty())
			real_path += server_ref._locations[best_index]._index;

		best_location_index = best_index;
		client_ref.best_mach = abs_Path(real_path);
	}
	else
		client_ref.best_mach = abs_Path(server_ref._root + url_path);
}

bool	Request::is_method_allowed()
{
	bool s_method = server_ref.get_method(client_ref.method);
	bool l_method = false;

	if (best_location_index != -1)
		l_method = server_ref._locations[best_location_index].get_method(client_ref.method);
	
	if (l_method || s_method)
		return true;
	return false;
}

void	Request::start_request()
{
	if (client_ref.statuc_code) return ;
	
	get_best_mach();

	if (client_ref.statuc_code) return ;
	
	if (client_ref.best_mach.empty())
	{
		client_ref.statuc_code = 404;
		client_ref.best_mach = server_ref._error_404;
	}
	else if (!is_method_allowed())
	{
		client_ref.statuc_code = 405;
	}
	else if (is_directory(client_ref.best_mach))
	{
		if (best_location_index == -1 || !server_ref._locations[best_location_index]._autoIndex)
			client_ref.statuc_code = 403;
		else
		{
			client_ref.statuc_code = 200;
			client_ref.is_dir = true;
		}
	}
	else
		client_ref.statuc_code = 200;
}