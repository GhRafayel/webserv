#include "Respons.hpp"

Respons::~Respons() {}

Respons::Respons(Server & S_ref, Client & C_ref) : StringUtils(),
    fun_map(),
    server_ref(S_ref),
	client_ref(C_ref),
	status_code(int_to_string(C_ref.statuc_code))
{
	init_fun_map();
	send_respons();
}

Respons::Respons(const Respons & obj) : StringUtils(),
	server_ref(obj.server_ref),
	client_ref(obj.client_ref),
	status_code(obj.status_code)
{
	this->ext = obj.ext;
	this->strim.clear();
	this->strim << obj.strim;
	this->body = obj.body;
}

Respons & Respons::operator=(const Respons & obj)
{
	if (this != &obj)
	{
		this->server_ref = obj.server_ref;
		this->client_ref = obj.client_ref;
		this->status_code = obj.status_code;
	}
	return *this;
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
	
	fun_map.insert(std::make_pair(200, &Respons::fun_200));
	fun_map.insert(std::make_pair(206, &Respons::fun_206));
	fun_map.insert(std::make_pair(301, &Respons::fun_301));
	fun_map.insert(std::make_pair(302, &Respons::fun_301));
	fun_map.insert(std::make_pair(400, &Respons::fun_400));
	fun_map.insert(std::make_pair(403, &Respons::fun_403));
	fun_map.insert(std::make_pair(404, &Respons::fun_404));
	fun_map.insert(std::make_pair(405, &Respons::fun_405));
	callFunctionByStatusCode(client_ref.statuc_code);
}

void Respons::fun_200()
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

void Respons::fun_206()
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

void Respons::fun_301()
{
	strim << client_ref.request.find("protocol")->second;
	strim << " " + int_to_string(client_ref.statuc_code) << " Moved Permanently\r\n";
	strim << "Location: " << client_ref.best_mach << "\r\n";
	strim << "Content-Length: 0\r\n";
	strim << "Connection: close \r\n\r\n";
	client_ref.outbuf = strim.str();
}

void Respons::fun_400()
{
	strim	<<	client_ref.request.find("protocol")->second << " 400 Bad Request";
	strim	<<	"Content-Length: 0\r\n";
	strim	<<	"Connection: close\r\n";
}

void Respons::fun_403()
{
	strim	<<	client_ref.request.find("protocol")->second << " 403 Forbidden";
	strim	<<	get_my_taype(ext) + "\r\n";
	strim	<<	"charset=utf-8\r\n";
	strim	<<	"Content-Length: 19\r\n";
	strim	<<	"Connection: close\r\n";
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

void Respons::fun_405()
{
	strim <<  client_ref.request.find("protocol")->second + " 405 Not Allowed\r\n";
	strim << "Server: my Server \r\n";
	strim << get_http_date() + "\r\n";
	strim << get_my_taype(ext) + "\r\n";
	strim << "Content-Length: 0 \r\n";
	strim << "Connection: keep-alive \r\n";
	client_ref.outbuf = strim.str();
}

void Respons::callFunctionByStatusCode(unsigned int fun_code)
{
	std::map<int, void (Respons::*) (void)>::iterator it = fun_map.find(fun_code);
	if (it == fun_map.end()) return ;
	(this->*(it->second))();
}
