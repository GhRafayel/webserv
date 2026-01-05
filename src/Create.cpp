#include "../hpp/Create.hpp"

Create::~Create() {}

Create::Create(Server & s_obj, Client & c_obj) : Response(s_obj, c_obj)
{
	inishialize_fun_map();
	create_response();
}

void	Create::inishialize_fun_map()
{
	size_t		post = client_ref.best_mach.rfind(".");
	if (post != std::string::npos)
		ext = client_ref.best_mach.substr(post, client_ref.best_mach.size());

	fun_map.insert(std::make_pair(201, &Create::fun_201));
	fun_map.insert(std::make_pair(403, &Create::fun_403));
	fun_map.insert(std::make_pair(413, &Create::fun_413));
	fun_map.insert(std::make_pair(500, &Create::fun_500));
}

void	Create::create_response()
{	
	std::map<std::string, std::string>::iterator it = client_ref.request.find("body");
	
	if (!is_method_allowed())
		client_ref.statuc_code = 405;
	else if (it->second.size() > static_cast<size_t>(server_ref._body_max_size))
		client_ref.statuc_code = 413;
	else
	{
		body = it->second;
		client_ref.statuc_code = 201;
	}
	callFunctionByStatusCode();
}

void	Create::fun_201()
{
	strim <<  client_ref.request.find("protocol")->second + " 201 Created\r\n";
	strim << "Server: my Server \r\n";
	strim << get_http_date() + "\r\n";
	strim << get_my_taype(ext) + "\r\n";
	strim << "Content-Length: " + int_to_string(body.size()) + " \r\n";
	strim << "Connection: close\r\n\r\n";
	strim << body;
	client_ref.outbuf = strim.str();
}

void Create::fun_413()
{
	strim <<  client_ref.request.find("protocol")->second + " 413 Request Entity Too Large\r\n";
	strim << "Server: my Server \r\n";
	strim << get_http_date() + "\r\n";
	strim << get_my_taype(ext) + "\r\n";
	strim << "Content-Length: 0 \r\n";
	strim << "Connection: close\r\n\r\n";
	client_ref.outbuf = strim.str();
}

void Create::fun_405()
{
	strim <<  client_ref.request.find("protocol")->second + " 405 Method Not Allowed\r\n";
	strim << "Server: my Server \r\n";
	strim << get_http_date() + "\r\n";
	strim << get_my_taype(ext) + "\r\n";
	strim << "Content-Length: 0 \r\n";
	strim << "Connection: close\r\n\r\n";
	client_ref.outbuf = strim.str();
}

void Create::fun_500()
{
	strim <<  client_ref.request.find("protocol")->second + " 500 Internal Server Error\r\n";
	strim << "Server: my Server \r\n";
	strim << get_http_date() + "\r\n";
	strim << get_my_taype(ext) + "\r\n";
	strim << "Content-Length: 0 \r\n";
	strim << "Connection: close\r\n\r\n";
	client_ref.outbuf = strim.str();
}

void Create::callFunctionByStatusCode()
{
	std::map<int, void (Create::*) (void)>::iterator it = fun_map.find(client_ref.statuc_code);
	if (it == fun_map.end())
		return ;
	(this->*(it->second))();
}
