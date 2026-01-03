#include "../hpp/Post.hpp"

Post::~Post() {}

Post::Post(Server & s_obj, Client & c_obj) : Response(s_obj, c_obj)
{
	size_t		post = client_ref.best_mach.rfind(".");
	if (post != std::string::npos)
		ext = client_ref.best_mach.substr(post, client_ref.best_mach.size());

	fun_map.insert(std::make_pair(201, &Post::fun_201));
	fun_map.insert(std::make_pair(405, &Post::fun_405));
	fun_map.insert(std::make_pair(404, &Post::fun_404));
	// fun_map.insert(std::make_pair(413, &Post::fun_413));
	fun_map.insert(std::make_pair(500, &Post::fun_500));
	create_response();
}

void	Post::create_response()
{	
	std::map<std::string, std::string>::iterator it = client_ref.request.find("body");
	
	if (!is_method_allowed())
		client_ref.statuc_code = 405;
	else if (client_ref.best_mach.empty())
		client_ref.statuc_code = 404;
	else if (!writable(client_ref.best_mach))
		client_ref.statuc_code = 500;
	else
	{
		if (it == client_ref.request.end())
			client_ref.statuc_code = 400;
		else
		{
			body = it->second;
			client_ref.statuc_code = 201;
		}
	}
	callFunctionByStatusCode(client_ref.statuc_code);
}

void	Post::fun_201()
{
	std::ofstream	file(client_ref.best_mach.c_str());
	file << body;
	file.close();
	strim	<<  "HTTP/1.1 201 Created";
	client_ref.outbuf = strim.str();
}

void Post::fun_404()
{
	strim	<< client_ref.request.find("protocol")->second << " 404 Not Found\r\n"
			<< "Server: my Server \r\n" + get_http_date() << "\r\n"
			<< "Content-Type: " << get_my_taype(ext) << "\r\n"
			<< "Content-Length: 0\r\n"
			<< "Connection: close\r\n\r\n";
	client_ref.outbuf = strim.str();
}

void Post::fun_405()
{
	strim <<  client_ref.request.find("protocol")->second + " 405 Not Allowed\r\n";
	strim << "Server: my Server \r\n";
	strim << get_http_date() + "\r\n";
	strim << get_my_taype(ext) + "\r\n";
	strim << "Content-Length: 0 \r\n";
	strim << "Connection: close\r\n\r\n";
	client_ref.outbuf = strim.str();
}


void Post::fun_500()
{
	strim <<  client_ref.request.find("protocol")->second + " 404 Not Found\r\n"
			<< "Server: my Server \r\n" + get_http_date() << "\r\n"
			<< "Content-Type: " << get_my_taype(ext) << "\r\n"
			<< "Content-Length: 0\r\n"
			<< "Connection: close\r\n\r\n";
	client_ref.outbuf = strim.str();
}

void Post::callFunctionByStatusCode(unsigned int fun_code)
{
	std::map<int, void (Post::*) (void)>::iterator it = fun_map.find(fun_code);
	if (it == fun_map.end())
		return ;
	(this->*(it->second))();
}
