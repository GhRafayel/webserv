#include "../hpp/Response.hpp"

Response::~Response() {}

Response::Response(Server & S_ref, Client & C_ref) : StringUtils(),
    server_ref(S_ref),
	client_ref(C_ref)
{

}

Response::Response(const Response & obj) : StringUtils(),
	server_ref(obj.server_ref),
	client_ref(obj.client_ref)
{
	this->strim.clear();
	this->strim << obj.strim.str();
}

Response & Response::operator=(const Response & obj)
{
	if (this != &obj)
	{
		this->server_ref = obj.server_ref;
		this->client_ref = obj.client_ref;
		this->strim.clear();
		this->strim << obj.strim.str();
	}
	return *this;
}

void Response::send_response()
{
	ssize_t n = 1;
	// while (n)
	// {
		n = send(client_ref.fd, client_ref.outbuf.data(), client_ref.outbuf.size(), 0);
		if (n > 0)
		{
			client_ref.outbuf.erase(0, n);
			//continue;
		}
		// else if (n < 0)
		// {
		// 	//_pollfds[index].events |= POLLOUT;
		// 	return;
		// }
	//}
}

std::string	Response::static_page()
{
	std::string str;
	DIR* dir = opendir(path.c_str());
	struct dirent* entry;
	while ((entry = readdir(dir)) != NULL)
	{
		str += "<div style='"
							"display:inline-block;"
							"margin:10px;"
							"padding:10px;"
							"text-align:center;"
							"border:1px solid #ccc;"
							"border-radius:8px;"
							"width:120px;'>";
		str += "<a href='/'";
		str += entry->d_name;
		str += "' style='text-decoration:none;color:black;'>";
		if (entry->d_type == DT_DIR)
			str += "<img src='../img/folder.jpg' width='40' height='40'><br>";
		else
			str += "<img src='../img/file.jpg' width='40' height='40'><br>";
		str += entry->d_name;
		str += "</a>";
		str += "</div>";
	}
	closedir(dir);
	client_ref.is_dir = true;
	return str;
}

void Response::check_cgi_status_code()
{
	if (client_ref.is_cgi && (client_ref.statuc_code == 500 || client_ref.statuc_code == 404) )
	{
		ext = ".html";
		if (client_ref.statuc_code == 500)
			path = server_ref._error_500;
		else if (client_ref.statuc_code == 404)
			path = server_ref._error_404;
	}
}

void	Response::create_header(const std::string & msg, bool val)
{
	std::string end = "\r\n";

	check_cgi_status_code();
	if(val)
		body = get_file_content(path);
	strim << client_ref.request.find("protocol")->second << (client_ref.statuc_code == 500 ? " 500 Internal Server Error" : msg) << end;
	strim << get_my_taype(ext) << end;
	strim << "Content_Length: " << body.size() << end;
	strim << "Server: my Server " << end;
	strim << "Data: " << get_http_date() << end;
	strim << "Conection:: close" << end << end;

	if(val)
		strim << body;
	client_ref.outbuf = strim.str();
}

bool	Response::is_method_allowed()
{
	bool s_method = server_ref.get_method(client_ref.method);
	bool l_method = false;

	if (client_ref.best_location_index != -1)
		l_method = server_ref._locations[client_ref.best_location_index].get_method(client_ref.method);
	
	if (l_method || s_method)
		return true;
	return false;
}