#include "../hpp/Response.hpp"

Response::~Response() {}

Response::Response(Server & S_ref, Client & C_ref) : StringUtils(),
    server_ref(S_ref),
	client_ref(C_ref)
{}

Response::Response(const Response & obj) : StringUtils(),
	server_ref(obj.server_ref),
	client_ref(obj.client_ref)
{
	this->strim.clear();
	this->strim << obj.strim;
}

Response & Response::operator=(const Response & obj)
{
	if (this != &obj)
	{
		this->server_ref = obj.server_ref;
		this->client_ref = obj.client_ref;
		this->strim.clear();
		this->strim <<  obj.strim;
	}
	return *this;
}

void Response::send_response()
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

std::string 	Response::error_page()
{
	std::stringstream st;

	st	<< "<!DOCTYPE html>" 
		<< "<html lang=\"en\"> " 
		<< "<head> "
				<< " <meta charset=\"UTF-8\"> " 
				<< " <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"> "
				<< " <title>Page Not Found | 404 Error</title> "
		<< "</head> "
		<< "<body> "
			<< "<div class=\"container\"> "
				"<h1 class=\"error-code\">" << int_to_string(client_ref.statuc_code)  << "</h1> "
				"<h2 class=\"error-title\">" << (client_ref.statuc_code != 404 ? "Server internal Error" : "Oops! Page Not Found") << "</h2> "
			<< "</div> "
		<< "</body>"
		<< "</html>";
	return st.str();
}
	
void	Response::create_header(const std::string & msg, bool val)
{
	std::string end = "\r\n";

	if(val)
		body = get_file_content(path);
	strim << client_ref.request.find("protocol")->second << msg << end;
	strim << get_my_taype(ext) << end;
	strim << "Content-Length: " << body.size() << end;
	strim << "Server: my Server " << end;
	strim << get_http_date() << end;
	strim << "Conection: close" << end;
	strim << end;
	if(val)
		strim << body;
	client_ref.outbuf = strim.str();
}

bool	Response::is_method_allowed()
{
	bool s_method = server_ref.get_method(client_ref.method);
	bool l_method = false;

	if (client_ref.best_location_index != -1)
		l_method = server_ref._locations[client_ref.best_location_index].get_method(client_ref.method);
	
	if (l_method || s_method)
		return true;
	return false;
}