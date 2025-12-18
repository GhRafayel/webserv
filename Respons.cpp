#include "Respons.hpp"

Respons::~Respons() {}

Respons::Respons(Server & S, Client & C) : StringUtils(),
    fun_map(),
    client_ref(C),
    server_ref(S) 
{
	init_fun_map();
	send_respons();
}

void Respons::send_respons()
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

void	Respons::init_fun_map()
{
	size_t		post = client_ref.best_mach.rfind(".");
	if (post != std::string::npos)
		ext = client_ref.best_mach.substr(post, client_ref.best_mach.size());
	fun_map.insert(std::make_pair(404, &Respons::fun_404));
	fun_map.insert(std::make_pair(200, &Respons::fun_200));
	fun_map.insert(std::make_pair(301, &Respons::fun_301));
	fun_map.insert(std::make_pair(302, &Respons::fun_301));
	fun_map.insert(std::make_pair(405, &Respons::fun_405));
	callFunctionByStatusCode(client_ref.statuc_code);
}

void Respons::fun_405()
{
	std::cout << get_my_taype(ext) << std::endl;
	strim <<  client_ref.request.find("protocol")->second + " 405 Not Allowed\r\n";
	strim << "Server: my Server \r\n";
	strim << get_http_date() + "\r\n";
	strim << get_my_taype(ext) + "\r\n";
	strim << "Content-Length: 0 \r\n";
	strim << "Connection: keep-alive \r\n";
	client_ref.outbuf = strim.str();
}

void Respons::fun_301()
{
	strim << client_ref.request.find("protocol")->second;
	strim << " " + int_to_string(client_ref.statuc_code) << " Moved Permanently\r\n";
	strim << "Location: " << client_ref.best_mach << "\r\n";
	strim << "Content-Length: 0\r\n";
	strim << "Connection: close \r\n\r\n";
	client_ref.outbuf = strim.str();
}

void Respons::fun_404()
{
	body = get_file_content(server_ref._error_404);
	strim	<< client_ref.request.find("protocol")->second << " 404 Not Found\r\n"
			<< "Server: my Server \r\n" + get_http_date() << "\r\n"
			<< "Content-Type: " << get_my_taype(ext) << "\r\n"
			<< "Content-Length: " << body.size() << "\r\n"
			<< "Connection: close\r\n\r\n" << body;
	client_ref.outbuf = strim.str();
}

void Respons::fun_200()
{
	std::map<std::string, std::string>::iterator it = client_ref.request.find("Connection");
	body = get_file_content(client_ref.best_mach);

	strim	<<  client_ref.request.find("protocol")->second  << " 200 OK\r\n"
		<<  get_my_taype(ext) << (it != client_ref.request.end() ? it->first + it->second : " ")
		<< "Content-Length: " << body.size()  <<  "\r\n\r\n" << body;
		client_ref.outbuf = strim.str();
}

void Respons::callFunctionByStatusCode(unsigned int fun_code)
{
	std::map<int, void (Respons::*) (void)>::iterator it = fun_map.find(fun_code);
	if (it == fun_map.end()) return ;
	(this->*(it->second))();
}
