#include "../hpp/Request.hpp"

Request::~Request() {}

Request::Request(Server & S_obj, Client & C_obj) : StringUtils(),
	server_ref(S_obj),
	client_ref(C_obj)
{
	parse_request();
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
	size_t post = client_ref.best_match.find("?");
	if (post != std::string::npos)
	{
		client_ref.query = client_ref.best_match.substr(post + 1);
		client_ref.best_match = client_ref.best_match.substr(0, post);
	}
	post = client_ref.best_match.find(".");
	if (post != std::string::npos && client_ref.best_location_index != -1)
	{
		std::string ext = client_ref.best_match.substr(post + 1);
		std::map<std::string, std::string> & temp = server_ref._locations[client_ref.best_location_index]._cgi;
		if (temp.find(ext) != temp.end())
		{
			client_ref.cgi_type = str_to_lower(temp.find(ext)->second);
			client_ref.cgibuf = client_ref.buffer;
			client_ref.is_cgi = true;
		}
	}
}

bool	Request::parse_request()
{
	client_ref.buffer = change_char(client_ref.buffer, '\t', ' ');
	std::vector<std::string> req = split(client_ref.buffer, "\r\n", true);

	if (req.size() < 3)
		return (client_ref.status_code = 400, false);
	
	std::vector<std::string> header = split(req[0], " ", true);
	if (header.size() < 3)
		return (client_ref.status_code = 400, false);
	client_ref.method = header[0];
	
	client_ref.request.insert(std::make_pair("url_path", header[1]));

	get_best_match(header[1]);
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
	if (client_ref.request.find("Cookie") == client_ref.request.end())
		client_ref.request.insert(std::make_pair("Cookie", ": session_id=" + int_to_string(rand() + client_ref.timeOut)));
	client_ref.is_dir = is_directory(abs_Path(client_ref.best_match));
	return true;
}

void Request::get_best_match(std::string & url_path)
{
	int				best_index = -1;
	std::string		best_loc;
	
	if (client_ref.request.find("url_path")->second == "/"){
		client_ref.best_match = server_ref._root + server_ref._index;
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
			client_ref.status_code = str_to_int(server_ref._locations[best_index]._return[0]);
			client_ref.best_match = server_ref._locations[best_index]._return[1];
			return ;
		}
		std::string relative_path = url_path.substr(best_loc.size());
		std::string real_path = server_ref._locations[best_index]._root + relative_path;

		if (!server_ref._locations[best_index]._index.empty() && relative_path.empty())
			real_path += server_ref._locations[best_index]._index;
		
		client_ref.best_location_index = best_index;
		client_ref.best_match = real_path;
	}
	else
		client_ref.best_match = server_ref._root + url_path;
}

