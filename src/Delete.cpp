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
	{
		client_ref.statuc_code = 403;
	}
	else
	{
		std::string	full_path = server_ref._root + "/" + client_ref.best_mach;
		if (!exists(full_path))
		{
			client_ref.statuc_code = 404;
		}
		else if (!writable(full_path))
		{
			client_ref.statuc_code = 403;
		}
		else
		{
			if (remove(full_path.c_str()) != 0)
			{
				client_ref.statuc_code = 500;
			}
			else
			{
				client_ref.statuc_code = 200;
			}
		}
	}
	callFunctionByStatusCode();
}

void	Delete::fun_200()
{
	strim	<<  client_ref.request.find("protocol")->second 
			<<  status_code + " OK File deleted successfully. \r\n"
			<< "Content-Length: 24\r\n"
			<< "Connection: close\r\n"
			<< "\r\n\r\n File deleted successfully.";
	client_ref.outbuf = strim.str();
}

void	Delete::fun_403()
{
	strim	<<  client_ref.request.find("protocol")->second 
			<<  status_code + " Forbidden\r\n"
			<< "Content-Length: 43\r\n"
			<< "Connection: close\r\n"
			<< "\r\n\r\n";
	client_ref.outbuf = strim.str();
}
void	Delete::fun_404()
{
	strim	<<  client_ref.request.find("protocol")->second 
			<<  status_code + " Not Found\r\n"
			<< "Content-Length: 34\r\n"
			<< "Connection: close\r\n"
			<< "\r\n\r\n";
	client_ref.outbuf = strim.str();
}

void	Delete::fun_500()
{
	strim	<<  client_ref.request.find("protocol")->second 
			<<  status_code + " Internal Server Error\r\n"
			<< "Content-Length: 58\r\n"
			<< "Connection: close\r\n"
			<< "\r\n\r\n";
	client_ref.outbuf = strim.str();
}

void	Delete::inishialize_fun_map()
{
	size_t		post = client_ref.best_mach.rfind(".");

	if (post != std::string::npos)
		ext = client_ref.best_mach.substr(post, client_ref.best_mach.size());

	fun_map.insert(std::make_pair(200, &Delete ::fun_200));
	fun_map.insert(std::make_pair(403, &Delete ::fun_403));
	fun_map.insert(std::make_pair(404, &Delete ::fun_404));
	fun_map.insert(std::make_pair(500, &Delete ::fun_500));
}

void Delete::callFunctionByStatusCode()
{
	std::map<int, void (Delete::*) (void)>::iterator it = fun_map.find(client_ref.statuc_code);
	if (it == fun_map.end()) return ;
	(this->*(it->second))();
}