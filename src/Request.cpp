#include "../hpp/Request.hpp"

Request::~Request() {}

Request::Request(Server & S_obj, Client & C_obj) : StringUtils(),
	server_ref(S_obj),
	client_ref(C_obj)
{
	pars_request();
	find_cgi();
}

Request::Request(const Request & obj) : StringUtils(),
	server_ref(obj.server_ref), client_ref(obj.client_ref)
{}

Request &	Request::operator = (const Request & obj)
{
	if (this != &obj)
	{
		this->server_ref = obj.server_ref;
		this->client_ref = obj.client_ref;
	}
	return *this;
}

void	Request::find_cgi()
{
	std::map<std::string, std::string>::iterator	it = client_ref.request.find("url_path");
	size_t											post;

	if (it != client_ref.request.end())
	{
		post = it->second.find("?");
		if (post != std::string::npos)
		{
			client_ref.question_mark = it->second.substr(post + 1);
			it->second = it->second.substr(0, post);
		}
		post = it->second.rfind(".");
		if (post != std::string::npos) {
		
			if (it->second.substr(post) == ".php") client_ref.is_cgi = true;
		}
	}
}

bool	Request::pars_request()
{
	client_ref.buffer = chang_char(client_ref.buffer, '\t', ' ');
	if (client_ref.buffer.find("Range:") == 0)
		return (client_ref.statuc_code = 206, client_ref.method = "GET", false);

	std::vector<std::string> req = split(client_ref.buffer, "\r\n", true);

	if (req.size() < 3)
		return (client_ref.statuc_code = 400, false);
	
	std::vector<std::string> header = split(req[0], " ", true);
	if (header.size() < 3)
		return (client_ref.statuc_code = 400, false);
	client_ref.method = header[0];
	
	client_ref.request.insert(std::make_pair("method", header[0]));
	client_ref.request.insert(std::make_pair("url_path", header[1]));
	client_ref.request.insert(std::make_pair("protocol", header[2]));

	get_best_mach(header[1]);
	for (size_t i = 1; i < req.size(); i++)
	{
		std::string	key, value;
		size_t 		post = req[i].find(":");

		if (i + 1 == req.size() && client_ref.method == "POST")
			client_ref.request.insert(std::make_pair("body", req[i]));
		else if (post != std::string::npos)
		{
			key = trim(req[i].substr(0, post), " ");
			value = trim(req[i].substr(post, req[i].size()), " ");
			client_ref.request.insert(std::make_pair(key, value + "\r\n"));
		}
	}
	client_ref.request.insert(std::make_pair("protocol", header[2]));
	return true;
}

void Request::get_best_mach(std::string & url_path)
{
	int				best_index = -1;
	std::string		best_loc;
	
	if (client_ref.request.find("url_path")->second == "/"){
		client_ref.best_mach = server_ref._root + server_ref._index;
		return ;
	}
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
		
		client_ref.best_location_index = best_index;
		client_ref.best_mach = real_path;
	}
	else
		client_ref.best_mach = server_ref._root + url_path;
}

