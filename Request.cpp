#include "Request.hpp"

Request::~Request() {}

//Request::Request() : StringUtils(), server_ref(Server()), client_ref(Client) {}

Request::Request(Server & S_obj, Client & C_obj) : StringUtils(),
	server_ref(S_obj),
	client_ref(C_obj),
	best_location_index(-1)
{
	analize_request();
	run();
}

Request::Request(const Request & obj) : StringUtils(),
	server_ref(obj.server_ref),
	client_ref(obj.client_ref)
{
	this->method = obj.method;
	this->url_path = obj.url_path;
	this->protocol = obj.protocol;
	this->request = obj.request;
	this->best_location_index = obj.best_location_index;
}

Request &	Request::operator = (const Request & obj)
{
	if (this != &obj)
	{
		this->server_ref = obj.server_ref;
		this->client_ref = obj.client_ref;
		this->method = obj.method;
		this->url_path = obj.url_path;
		this->protocol = obj.protocol;
		this->request = obj.request;
		this->best_location_index = obj.best_location_index;
	}
	return *this;
}

void	Request::analize_request()
{
	std::vector<std::string> req = split(client_ref.buffer, "\r\n", true);
	std::vector<std::string> header = split(req[0], " ", true);
	
	this->method = header[0];
	this->url_path = header[1];
	this->protocol = header[2];

	for (size_t i = 1; i < req.size(); i++)
	{
		size_t post = req[i].find(":");

		std::string key = trim(req[i].substr(0, post), " ");

		std::string value = trim(req[i].substr(post, req[i].size()), " ");

		request.insert(std::make_pair(key, value + "\r\n"));
	}

}

bool	Request::is_directory()
{
	DIR *dir = opendir(client_ref.best_mach.c_str());

	if (dir == NULL) return false;
	closedir(dir);
	return true;
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
    int	best_index = -1;
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
	if (best_location_index != -1 && server_ref._locations[best_location_index].get_method(method))
		return true;
	else if (server_ref._methods.find(method) != server_ref._methods.end())
		return true;
	return false;
}

void	Request::run()
{
	get_best_mach();

	if (client_ref.best_mach.empty())
	{
		client_ref.statuc_code = 404;
		client_ref.best_mach = server_ref._error_404;
		return ;
	}
	if (!is_method_allowed())
	{
		client_ref.statuc_code = 405;
		return ;
	}
	if (is_directory())
	{
		if (best_location_index == -1)
			client_ref.statuc_code = 403;
		else if (!server_ref._locations[best_location_index]._autoIndex)
		{
			client_ref.statuc_code = 403;
			return ;
		}
		else
		{
			client_ref.statuc_code = 200;
			return ;
		}
	}
	
	std::ostringstream t;

	std::string ext, body;

	size_t		post = client_ref.best_mach.rfind(".");

	if (post != std::string::npos)
		ext = client_ref.best_mach.substr(post, client_ref.best_mach.size());

	try
	{
		body = get_file_content(client_ref.best_mach);
		t	<<  protocol << "200 OK\r\n"
			<<  get_my_taype(ext)
			<<	request.find("Connection")->first
			<< 	request.find("Connection")->second
			<< "Content-Length: " << body.size()  <<  "\r\n\r\n" << body;
		client_ref.outbuf = t.str();
	}
	catch(const std::exception& e)
	{
		body = get_file_content(server_ref._error_404);

		std::cout << get_my_taype(ext) << std::endl;
		t << protocol << " 404 Not Found\r\n"
		<< "Content-Type: " << get_my_taype(ext)
		<< "Content-Length: " <<  body.size()  << "\r\n"
		<< "Connection: close\r\n\r\n" << body;
		client_ref.outbuf = t.str();
	}
}