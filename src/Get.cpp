#include "../hpp/Get.hpp"

Get::~Get() {}

Get::Get(Server & s_obj, Client & c_obj) : Response(s_obj, c_obj)
{
	inishialize_fun_map();
	if (client_ref.statuc_code)
		callFunctionByStatusCode();
	create_response();
}

void	Get::create_response() {

	if (!is_method_allowed())
		client_ref.statuc_code = 405;
	else if (path.empty())
	{
		client_ref.statuc_code = 404;
		client_ref.best_mach = server_ref._error_404;
	}
	else if (is_directory(path))
	{
		if (client_ref.best_location_index == -1 || !server_ref._locations[client_ref.best_location_index]._autoIndex)
			client_ref.statuc_code = 403;
		else
		{
			client_ref.statuc_code = 200;
			client_ref.is_dir = true;
		}
	}
	else
		client_ref.statuc_code = 200;
	callFunctionByStatusCode();
}

void	Get::inishialize_fun_map() {

	path = abs_Path(client_ref.best_mach);
	size_t		post = client_ref.best_mach.rfind(".");
	if (post != std::string::npos)
		ext = client_ref.best_mach.substr(post);
	fun_map.insert(std::make_pair(200, &Get::fun_200));
	fun_map.insert(std::make_pair(206, &Get::fun_200));
	fun_map.insert(std::make_pair(301, &Get::fun_301));
	fun_map.insert(std::make_pair(302, &Get::fun_301));
	fun_map.insert(std::make_pair(400, &Get::fun_400));
	fun_map.insert(std::make_pair(403, &Get::fun_403));
	fun_map.insert(std::make_pair(404, &Get::fun_404));
	fun_map.insert(std::make_pair(405, &Get::fun_405));
}

void Get::callFunctionByStatusCode()
{
	std::map<int, void (Get::*) (void)>::iterator it = fun_map.find(client_ref.statuc_code);
	if (it == fun_map.end()) return ;
	(this->*(it->second))();
}

void Get::fun_200()
{
	body = get_file_content(path);

	strim	<<  client_ref.request.find("protocol")->second << " 200 OK\r\n"
			<<	get_my_taype(ext) << "\r\n"
			<< 	"Content-Length: " << body.size() 
			<<  "Server: my Server \r\n"
			<<  "Date: " << get_http_date() << "\r\n"
			<< 	"Connection: close\r\n\r\n" << body;
	client_ref.outbuf = strim.str();
}

void Get::fun_206()
{
	if (!readable(path))
	{
		fun_403();
		return ;
	}
	std::vector<std::string> temp = Range_pars(client_ref.buffer);
	if (temp.empty()) 
	{
		fun_400();
		return ;
	}
	int	start = str_to_int(temp[0]);
	int	end = (temp[1].empty() ? -1 : str_to_int(temp[1]));
	if (end == -1)
		body = get_file_content(path);
	else
	{
		body = get_file_content(path, start, end - start + 1);
	}
 
	strim	<<  client_ref.request.find("protocol")->second << " 206 Partial Content\r\n" 
			<<  get_my_taype(ext) << "\r\n"
			<< 	"Content-Range: bytes " << start << "-" << end << "/" << body.size() << "\r\n"
			<< 	"Content-Length: " << body.size() << "\r\n"
			<< "Server: my Server \r\n"
			<< 	"Date: " << get_http_date() << "\r\n"
		 	<< 	"Connection: close \r\n\r\n" << body;
	client_ref.outbuf = strim.str();
}

void Get::fun_301()
{
	strim	<< client_ref.request.find("protocol")->second << " 301 Moved Permanently\r\n"
			<<  get_my_taype(ext) << "\r\n"
			<< 	"Content-Length: 0\r\n"
			<<  "Server: my Server \r\n"
			<< 	"Location: " << path << "\r\n"
			<< 	"Date: " << get_http_date() << "\r\n"
			<< 	"Connection: close \r\n\r\n";
	client_ref.outbuf = strim.str();
}

void Get::fun_400()
{
	strim	<<	client_ref.request.find("protocol")->second << " 400 Bad Request\r\n"
			<<  get_my_taype(ext) << "\r\n"
			<<	"Content-Length: 0\r\n"
			<<  "Server: my Server \r\n"
			<<	"Date: " << get_http_date() << "\r\n"
			<<	"Connection: close\r\n";
	client_ref.outbuf = strim.str();
}

void Get::fun_403()
{
	strim	<<	client_ref.request.find("protocol")->second << " 403 Forbidden\r\n"
			<<  get_my_taype(ext) << "\r\n"
			<<	"Content-Length: 19\r\n"
			<<  "Server: my Server \r\n"
			<<	"Date: " << get_http_date() << "\r\n"
			<<	"charset=utf-8\r\n"
			<<	"Connection: close\r\n";
	client_ref.outbuf = strim.str();
}

void Get::fun_404()
{

	size_t		post = client_ref.best_mach.rfind(".");
	if (post != std::string::npos)
		ext = client_ref.best_mach.substr(post);
	body = get_file_content(server_ref._error_404);

	strim	<< client_ref.request.find("protocol")->second  << " 404 Not Found\r\n"
			<< get_my_taype(ext) << "\r\n"
			<< "Content-Length: " << body.size() << "\r\n"
			<< "Server: my Server \r\n"
			<< "Date: " << get_http_date() << "\r\n"
			<< "Connection: close\r\n\r\n" << body;
	client_ref.outbuf = strim.str();
}	

void Get::fun_405()
{
	strim 	<<  client_ref.request.find("protocol")->second << " 405 Not Allowed\r\n"
			<<  get_my_taype(ext) << "\r\n"
			<< "Content-Length: 0 \r\n"
			<< "Server: my Server \r\n"
			<< "Date: " << get_http_date() << "\r\n"
			<< "Connection: close \r\n\r\n";
	client_ref.outbuf = strim.str();
}