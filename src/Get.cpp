#include "../hpp/Get.hpp"

Get::~Get() {}

Get::Get(Server & s_obj, Client & c_obj) : Response(s_obj, c_obj)
{
  size_t		post = client_ref.best_mach.rfind(".");
	if (post != std::string::npos)
		ext = client_ref.best_mach.substr(post, client_ref.best_mach.size());

	fun_map.insert(std::make_pair(200, &Get::fun_200));
	fun_map.insert(std::make_pair(206, &Get::fun_206));
	fun_map.insert(std::make_pair(301, &Get::fun_301));
	fun_map.insert(std::make_pair(302, &Get::fun_301));

	//fun_map.insert(std::make_pair(401, &Get::fun_401));

	fun_map.insert(std::make_pair(400, &Get::fun_400));
	fun_map.insert(std::make_pair(403, &Get::fun_403));
	fun_map.insert(std::make_pair(404, &Get::fun_404));
	fun_map.insert(std::make_pair(405, &Get::fun_405));

	// fun_map.insert(std::make_pair(411, &Get::fun_411));
	// fun_map.insert(std::make_pair(413, &Get::fun_413));
	// fun_map.insert(std::make_pair(414, &Get::fun_414));
	// fun_map.insert(std::make_pair(500, &Get::fun_500));
	// fun_map.insert(std::make_pair(503, &Get::fun_503));

	callFunctionByStatusCode(client_ref.statuc_code);
}

void Get::fun_200()
{
	std::map<std::string, std::string>::iterator it = client_ref.request.find("Connection");
	body = get_file_content(client_ref.best_mach);

	strim	<<  client_ref.request.find("protocol")->second
			<<  status_code + " OK\r\n"
			<<  get_my_taype(ext) << (it != client_ref.request.end() ? it->first + it->second : "application/octet-stream\r\n")
			<< "Content-Length: " << body.size()
			<<  "\r\n\r\n" << body;
		client_ref.outbuf = strim.str();
}

void Get::fun_206()
{
	if (!readable(client_ref.best_mach))
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
		body = get_file_content(client_ref.best_mach);
	else
	{
		body = get_file_content(client_ref.best_mach, start, end - start + 1);
	}

	strim	<<  client_ref.request.find("protocol")->second
			<< " 206 Partial Content\r\n" <<  get_my_taype(ext)
			<< "Content-Range: bytes " << start << "-" << end << "/" << body.size() << "\r\n"
			<< "Content-Length: " << body.size() << "\r\n"
			<< "Connection: keep-alive\r\n\r\n" << body;
			client_ref.outbuf = strim.str();
}

void Get::fun_301()
{
	strim << client_ref.request.find("protocol")->second;
	strim << " " + int_to_string(client_ref.statuc_code) << " Moved Permanently\r\n";
	strim << "Location: " << client_ref.best_mach << "\r\n";
	strim << "Content-Length: 0\r\n";
	strim << "Connection: close \r\n\r\n";
	client_ref.outbuf = strim.str();
}

void Get::fun_400()
{
	strim	<<	client_ref.request.find("protocol")->second << " 400 Bad Request";
	strim	<<	"Content-Length: 0\r\n";
	strim	<<	"Connection: close\r\n";
}

void Get::fun_403()
{
	strim	<<	client_ref.request.find("protocol")->second << " 403 Forbidden";
	strim	<<	get_my_taype(ext) + "\r\n";
	strim	<<	"charset=utf-8\r\n";
	strim	<<	"Content-Length: 19\r\n";
	strim	<<	"Connection: close\r\n";
	client_ref.outbuf = strim.str();
}

void Get::fun_404()
{
	body = get_file_content(server_ref._error_404);
	strim	<< client_ref.request.find("protocol")->second << " 404 Not Found\r\n"
			<< "Server: my Server \r\n" + get_http_date() << "\r\n"
			<< "Content-Type: " << get_my_taype(ext) << "\r\n"
			<< "Content-Length: " << body.size() << "\r\n"
			<< "Connection: close\r\n\r\n" << body;
	client_ref.outbuf = strim.str();
}

void Get::fun_405()
{
	strim <<  client_ref.request.find("protocol")->second + " 405 Not Allowed\r\n";
	strim << "Server: my Server \r\n";
	strim << get_http_date() + "\r\n";
	strim << get_my_taype(ext) + "\r\n";
	strim << "Content-Length: 0 \r\n";
	strim << "Connection: keep-alive \r\n";
	client_ref.outbuf = strim.str();
}

void Get::callFunctionByStatusCode(unsigned int fun_code)
{
	std::map<int, void (Get::*) (void)>::iterator it = fun_map.find(fun_code);
	if (it == fun_map.end()) return ;
	(this->*(it->second))();
}

