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

	fun_map.insert(std::make_pair(200, &Create::fun_200));
	fun_map.insert(std::make_pair(405, &Create::fun_405));
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

void	Create::fun_200()
{
	strim	<< client_ref.request.find("protocol")->second 
			<< " " << int_to_string(client_ref.statuc_code) << " OK\r\n"
			<< "Server: my Server \r\n"
			<< get_http_date() + "\r\n"
			<< get_my_taype(ext) + "\r\n"
			<< "Content-Length: " + int_to_string(body.size()) + " \r\n"
			<< "Connection: close\r\n\r\n" << body;
	client_ref.outbuf = strim.str();
}

void Create::fun_413()
{
	strim	<<  client_ref.request.find("protocol")->second 
			<<  " " << int_to_string(client_ref.statuc_code)
			<< " Request Entity Too Large\r\n"
			<< "Server: my Server \r\n"
	 		<< get_http_date() + "\r\n"
	 		<< get_my_taype(ext) + "\r\n"
	 		<< "Content-Length: 0 \r\n"
	 		<< "Connection: close\r\n\r\n";
	client_ref.outbuf = strim.str();
}

void Create::fun_405()
{
	strim	<< client_ref.request.find("protocol")->second 
			<< " " << int_to_string(client_ref.statuc_code) 
			<< " Method Not Allowed\r\n"
			<< "Server: my Server \r\n"
			<< get_http_date() + "\r\n"
			<< get_my_taype(ext) + "\r\n"
			<< "Content-Length: 0 \r\n"
			<< "Connection: close\r\n\r\n";
	client_ref.outbuf = strim.str();
}

void Create::fun_500()
{
	strim	<<  client_ref.request.find("protocol")->second
			<< " " << int_to_string(client_ref.statuc_code)
			<< " Internal Server Error\r\n"
			<< "Server: my Server \r\n"
			<< get_http_date() + "\r\n"
			<< get_my_taype(ext) + "\r\n"
			<< "Content-Length: 0 \r\n"
			<< "Connection: close\r\n\r\n";
	client_ref.outbuf = strim.str();
}

void Create::callFunctionByStatusCode()
{
	std::map<int, void (Create::*) (void)>::iterator it = fun_map.find(client_ref.statuc_code);
	if (it == fun_map.end())
		return ;
	(this->*(it->second))();
}
