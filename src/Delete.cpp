#include "../hpp/Delete.hpp"

Delete::~Delete () {}

Delete::Delete (Server & s_obj, Client & c_obj) : Response(s_obj, c_obj)
{
	inishialize_fun_map();
	create_response();
}

void	Delete::create_response()
{
	if (!is_method_allowed())
		client_ref.statuc_code = 405;
	else
	{
		if (!exists(path, '\0'))
			client_ref.statuc_code = 404;
		else if (!writable(path) || remove(path.c_str()) != 0)
			client_ref.statuc_code = 500;
		else
			client_ref.statuc_code = 200;
	}
	callFunctionByStatusCode();
}

void	Delete::fun_200()
{
	strim	<<  client_ref.request.find("protocol")->second << " 200 OK\r\n"
			<<	get_my_taype(ext) << "\r\n"
			<< "Content-Length: 0\r\n"
			<< "Date: " << get_http_date() << "\r\n"
			<< "Connection: close\r\n\r\n";
	client_ref.outbuf = strim.str();
}

void	Delete::fun_405()
{
	strim	<<  client_ref.request.find("protocol")->second << " 405 Forbidden\r\n"
			<<	get_my_taype(ext) << "\r\n"
			<< "Date: " << get_http_date() << "\r\n"
			<< "Content-Length: 0\r\n"
			<< "Connection: close\r\n\r\n";
	client_ref.outbuf = strim.str();
}
void	Delete::fun_404()
{
	strim	<<  client_ref.request.find("protocol")->second << " 404 Not Found\r\n"
			<<	get_my_taype(ext) << "\r\n"
			<< "Date: " << get_http_date() << "\r\n"
			<< "Content-Length: 0\r\n"
			<< "Connection: close\r\n\r\n";
	client_ref.outbuf = strim.str();
}

void	Delete::fun_500()
{
	strim	<<  client_ref.request.find("protocol")->second << "500 Internal Server Error\r\n"
			<<	get_my_taype(ext) << "\r\n"
			<< "Date: " << get_http_date() << "\r\n"
			<< "Content-Length: 58\r\n"
			<< "Connection: close\r\n\r\n";
	client_ref.outbuf = strim.str();
}

void	Delete::inishialize_fun_map()
{
			path = abs_Path(client_ref.best_mach);
	size_t	post = client_ref.best_mach.rfind(".");

	if (post != std::string::npos)
		ext = client_ref.best_mach.substr(post, client_ref.best_mach.size());

	fun_map.insert(std::make_pair(200, &Delete ::fun_200));
	fun_map.insert(std::make_pair(405, &Delete ::fun_405));
	fun_map.insert(std::make_pair(404, &Delete ::fun_404));
	fun_map.insert(std::make_pair(500, &Delete ::fun_500));
}

void Delete::callFunctionByStatusCode()
{
	std::map<int, void (Delete::*) (void)>::iterator it = fun_map.find(client_ref.statuc_code);

	if (it == fun_map.end()) return ;
	(this->*(it->second))();
}