#include "Request.hpp"

Request::~Request() {}

//Request::Request() : StringUtils(), server_ref(Server()), client_ref(Client) {}

Request::Request(Server & S_obj, Client & C_obj) :
	StringUtils(), server_ref(S_obj), client_ref(C_obj), best_location_index(-1)
{
	analize_request();
	run();
}

Request::Request(const Request & obj) :
	StringUtils(), server_ref(obj.server_ref), client_ref(obj.client_ref)
{
	this->method = obj.method;
	this->path = obj.path;
	this->protocol = obj.protocol;
	this->request = obj.request;
	this->best_location_index = obj.best_location_index;
}
std::string Request::getProtocol(){
	return this->protocol;
};

std::string	Request::getMethod(){
	return this->method;	
};

std::string	Request::getPath() {
	return this->path;
};

std::map<std::string, std::string> & Request::getRequest() {
	return this->request;
}

Request &	Request::operator = (const Request & obj)
{
	if (this != &obj)
	{
		this->server_ref = obj.server_ref;
		this->client_ref = obj.client_ref;
		this->method = obj.method;
		this->path = obj.path;
		this->protocol = obj.protocol;
		this->request = obj.request;
		this->best_location_index = obj.best_location_index;
	}
	return *this;
}

void	Request::analize_request()
{
	std::vector<std::string> req = split(client_ref.buffer, "\r\n", true);
	std::cout << req[0] << std::endl;
	std::vector<std::string> header = split(req[0], " ", true);
	this->method = header[0];
	this->path = header[1];
	this->protocol = header[2];
	if (client_ref.first_path.empty())
		client_ref.first_path = path;
	for (size_t i = 1; i < req.size(); i++)
	{
		size_t post = req[i].find(":");

		std::string key = trim(req[i].substr(0, post), " ");

		std::string value = trim(req[i].substr(post, req[i].size()), " ");

		request.insert(std::make_pair(key, value + "\r\n"));
	}
}

std::string	Request::is_defoult_location(const std::string & loc)
{
	if (loc == "/")
	{
		return server_ref._root + server_ref._index;
	}
		
	if (loc.substr(1, loc.size()).find("/") == std::string::npos)
	{
		return "www/public/" + loc;
	}
	return "";
}
std::string Request::get_best_mach(const std::string & url_path)
{
    size_t best_index = -1;
    std::string best_loc = is_defoult_location(url_path);
	if (!best_loc.empty()) return best_loc;
	
    for (size_t i = 0; i < server_ref._locations.size(); i++)
    {
        std::string loc = server_ref._locations[i]._location;
        if (url_path.find(loc) == 0 && loc.size() > best_loc.size())
        {
            best_loc = loc;
            best_index = i;
        }
    }

    if (best_index != (size_t)-1)
    {
        std::string relative_path = url_path.substr(best_loc.size());
        std::string real_path = server_ref._locations[best_index]._root + relative_path;

        if (!server_ref._locations[best_index]._index.empty() && relative_path.empty())
            real_path += server_ref._locations[best_index]._index;

        best_location_index = best_index;
        return real_path;
    }

    return server_ref._root + url_path;
}

void	Request::run()
{
	std::string path = getPath();
	std::string	best_mach = get_best_mach(path);
	best_mach = abs_Path(best_mach);
	std::ostringstream t;
	std::string ext;
	std::string body;
	size_t		post = best_mach.rfind(".");

	if (post != std::string::npos)
		ext = best_mach.substr(post, best_mach.size());
	try
	{
		body = get_file_content(best_mach);
		t	<<  getProtocol() << "200 OK\r\n"
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
		t << getProtocol() << " 404 Not Found\r\n"
		<< "Content-Type: " << get_my_taype(ext)
		<< "Content-Length: " <<  body.size()  << "\r\n"
		<< "Connection: close\r\n\r\n" << body;
		client_ref.outbuf = t.str();
	}
}