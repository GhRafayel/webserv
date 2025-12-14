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
	size_t		post = client_ref.best_mach.rfind(".");
	if (post != std::string::npos)
		ext = client_ref.best_mach.substr(post, client_ref.best_mach.size());
	callFunctionByStatusCode(client_ref.statuc_code);
}

void	Respons::init_fun_map()
{
	fun_map.insert(std::make_pair(404, &Respons::fun_404));
	fun_map.insert(std::make_pair(200, &Respons::fun_200));
}

void Respons::fun_404()
{
	body = get_file_content(server_ref._error_404);
	strim << client_ref.request.find("protocol")->second << "404 Not Found\r\n";
	strim << "Content-Type: " << get_my_taype(ext);
	strim << "Content-Length: " << body.size() << "\r\n";
	strim << "Connection: close\r\n\r\n" << body;
	client_ref.outbuf = strim.str();
}

void Respons::fun_200()
{
	body = get_file_content(client_ref.best_mach);
	strim	<<  client_ref.request.find("protocol")->second  << "200 OK\r\n"
		<<  get_my_taype(ext)
		<<	client_ref.request.find("Connection")->first
		<< 	client_ref.request.find("Connection")->second
		<< "Content-Length: " << body.size()  <<  "\r\n\r\n" << body;
		client_ref.outbuf = strim.str();
}

void Respons::callFunctionByStatusCode(unsigned int fun_code)
{
	std::map<int, void (Respons::*) (void)>::iterator it = fun_map.find(fun_code);
	if (it == fun_map.end()) return ;
	(this->*(it->second))();
}

/*
	if (post != std::string::npos)
		ext = client_ref.best_mach.substr(post, client_ref.best_mach.size());

	try
	{
		
	}
	catch(const std::exception& e)
	{
		body = get_file_content(server_ref._error_404);

		std::cout << get_my_taype(ext) << std::endl;
		t << protocol << " 404 Not Found\r\n"
		<< "Content-Type: " << get_my_taype(ext)
		<< "Content-Length: " <<  body.size()  << "\r\n"
		<< "Connection: close\r\n\r\n" << body;
		client_ref.outbuf = t.str();
	}

*/