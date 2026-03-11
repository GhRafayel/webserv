#include "../hpp/Response.hpp"

Response::~Response() {}

Response::Response(Server & S_ref, Client & C_ref) : StringUtils(),  func_map(), end_line("\r\n"),
	server_ref(S_ref),
	client_ref(C_ref)
{
	init();
}

Response::Response(const Response & obj) : StringUtils(), func_map(), end_line("\r\n"),
	server_ref(obj.server_ref),
	client_ref(obj.client_ref)
{
	this->strim.clear();
	this->strim << obj.strim.str();
	this->func_map = obj.func_map;
	this->body = obj.body;
}

Response & Response::operator=(const Response & obj)
{
	if (this != &obj)
	{
		this->server_ref = obj.server_ref;
		this->client_ref = obj.client_ref;
		this->strim.clear();
		this->strim << obj.strim.str();
		this->end_line = obj.end_line;
	}
	return *this;
}

void Response::send_response()
{
	std::map<int, void (Response::*) (void)>::iterator it = func_map.find(client_ref.statuc_code);
	if (it == func_map.end()) 
		(this->*func_map[400])();
	else
		(this->*func_map[it->first])();
	// to_send();
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

void	Response::init() {
		func_map.insert(std::make_pair(200, &Response::fun_200));
		func_map.insert(std::make_pair(206, &Response::fun_206));
		func_map.insert(std::make_pair(301, &Response::fun_301));
		func_map.insert(std::make_pair(400, &Response::fun_400));
		func_map.insert(std::make_pair(404, &Response::fun_404));
		func_map.insert(std::make_pair(403, &Response::fun_403));
		func_map.insert(std::make_pair(405, &Response::fun_405));
		func_map.insert(std::make_pair(423, &Response::fun_423));
		func_map.insert(std::make_pair(500, &Response::fun_500));
		func_map.insert(std::make_pair(200200, &Response::fun_200200));
}

void	Response::fun_200200(){
	body = static_page();
	strim << "HTTP/1.0 200 ok" << end_line;
	ext = ".html";
	create_header();
	strim << body << end_line;
	client_ref.outbuf = strim.str();
};

void	Response::fun_200() {
	
	// if (client_ref.is_cgi) 
	// {
	// 	client_ref.outbuf = strim.str();
	// 	return;
	// 	// body = client_ref.outbuf;
	// 	// close(client_ref.fd);
	// 	// ext = ".html";
	// }
	// else
	body = get_file_content(abs_Path(client_ref.best_mach));
	strim << "HTTP/1.0 200 ok" << end_line;
	create_header();
	strim << body << end_line;
	client_ref.outbuf = strim.str();
};

void	Response::fun_206(){
	
};

void	Response::fun_301() {
	strim << "HTTP/1.0 " << int_to_string(client_ref.statuc_code) << " Moved Permanently" << end_line;
	strim << "Location: "  << client_ref.best_mach << end_line;
	create_header();
	client_ref.outbuf = strim.str();
};

void	Response::fun_400(){
	strim << "HTTP/1.0 400 Bed Request" << end_line;
	create_header();
	client_ref.outbuf = strim.str();
};

void	Response::fun_403(){
	strim << "HTTP/1.0 403 Forbidden" << end_line;
	create_header();
	client_ref.outbuf = strim.str();
};

void	Response::fun_404(){
	ext = ".html";
	client_ref.best_mach = abs_Path(server_ref._error_404);
	body = get_file_content(client_ref.best_mach);
	strim << "HTTP/1.0 404 Not Found" << end_line;
	create_header();
	strim << body << end_line;
	client_ref.outbuf = strim.str();
};

void	Response::fun_405(){
	strim << "HTTP/1.0 405 Not Allowed" << end_line;
	create_header();
	client_ref.outbuf = strim.str();
};

void	Response::fun_423(){
	strim << "HTTP/1.0 423 Payload Too Large" << end_line;
	create_header();
	client_ref.outbuf = strim.str();
};

void	Response::fun_500(){
	ext = ".html";
	body = get_file_content(abs_Path(server_ref._error_500));
	strim << "HTTP/1.0 500 Internal Server Error" << end_line;
	create_header();
	client_ref.outbuf = strim.str();
};

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

void	Response::create_header()
{
	strim << get_my_taype(ext) << end_line;
	strim << "Content_Length: " << body.size() << end_line;
	strim << "Server: " << server_ref._server_name << " " << end_line;
	strim << "Data: " << get_http_date() << end_line;
	strim << "Conection:: close" << end_line << end_line;
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