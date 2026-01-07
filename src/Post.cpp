#include "../hpp/Post.hpp"

Post::~Post() {}

Post::Post(Server & s_obj, Client & c_obj) : Response(s_obj, c_obj), env(NULL)
{
	inishialize_fun_map();
	create_response();
}

void	Post::inishialize_fun_map()
{
	path = abs_Path(client_ref.best_mach);
	size_t		post = path.rfind(".");
	if (post != std::string::npos)
		ext = path.substr(post, path.size());

	fun_map.insert(std::make_pair(200, &Post::fun_200));
	fun_map.insert(std::make_pair(405, &Post::fun_405));
	fun_map.insert(std::make_pair(404, &Post::fun_404));
	fun_map.insert(std::make_pair(413, &Post::fun_413));
	fun_map.insert(std::make_pair(500, &Post::fun_500));
}

bool Post::check_size()
{
	int size = client_ref.request.find("body")->second.size();

	if (client_ref.best_location_index != -1)
	{
		if (server_ref._locations[client_ref.best_location_index]._max_body_size < size || server_ref._body_max_size < size)
			return false;
	}
	return true;
}

bool	m_fork(int fd[2])
{
	(void)fd;
	pid_t pid = fork();
	if (pid != 0)
		return false;
	std::cout << "hello child process is open " << std::endl;
	

	// if (fd[0] != STDIN_FILENO)
	// {
	// 	dup2(fd[0], STDIN_FILENO);
	// }
	// if (fd[1] != STDOUT_FILENO)
	// {
	// 	dup2(fd[1], STDOUT_FILENO);
	// }
	
	//execve();
	return true;
}
bool	Post::is_cgi()
{
	int	fd[2];
	// if (pipe(fd) == -1)
	// {
	// 	std::cerr << "PIPE_FAILURE" << std::endl;
	// 	return  false;
	// }
	return (m_fork(fd));
	
}
#include <sys/wait.h>
void	Post::create_response()
{	
	std::map<std::string, std::string>::iterator it = client_ref.request.find("body");
	
	if (!is_method_allowed())
		client_ref.statuc_code = 405;
	else if (is_cgi())
	{
		int status = -1;
		wait(&status);
	}
	else if (!exists(path))
		client_ref.statuc_code = 404;
	else if (check_size())
		client_ref.statuc_code = 413;
	else
	{
		body = it->second;
		client_ref.statuc_code = 200;
		std::ofstream file(abs_Path(client_ref.best_mach).c_str());
		if (!writable(path))
				client_ref.statuc_code = 500;
		file << body;
	}
	callFunctionByStatusCode();
}

void	Post::fun_200()
{
	strim	<<  "HTTP/1.1 200 ok\n\r"
			<<	get_my_taype(ext) << "\r\n"
			<<	"Server: my Server\r\n"
			<<	"Content-Length: 0 \r\n"
			<< "Date: " << get_http_date() << "\r\n"
         	<< "Connection: close\r\n\r\n";
	client_ref.outbuf = strim.str();
}

void Post::fun_404()
{
    const std::string body = "404 Not Found - the requested resource does not exist.\r\n";
    strim << client_ref.request.find("protocol")->second << " 404 Not Found\r\n"
          << "Content-Type: text/plain\r\n"
          << "Server: my Server\r\n"
          << "Content-Length: " << body.size() << "\r\n"
          << "Date: " << get_http_date() << "\r\n"
          << "Connection: close\r\n\r\n"
          << body;

    client_ref.outbuf = strim.str();
}

void Post::fun_405()
{ 
	strim 	<<  client_ref.request.find("protocol")->second <<" 405 Not Allowed\r\n";
	strim	<<	"Server: my Server \r\n";
	strim	<<	"Date: " << get_http_date() << "\r\n";
	strim	<< 	"Connection: close\r\n\r\n";
	client_ref.outbuf = strim.str();
}

void Post::fun_413()
{ 
	strim 	<<  client_ref.request.find("protocol")->second << " 413 Payload Too Large\r\n";
	strim	<<	"Server: my Server \r\n";
	strim	<<	"Date: " << get_http_date() << "\r\n";
	strim	<< 	"Connection: close\r\n\r\n";
	client_ref.outbuf = strim.str();
}

void Post::fun_500()
{ 
	strim 	<<  client_ref.request.find("protocol")->second << " 500 Internal Server Error\r\n";
	strim	<<	"Server: my Server \r\n";
	strim	<<	"Date: " << get_http_date() << "\r\n";
	strim	<< 	"Connection: close\r\n\r\n";
	client_ref.outbuf = strim.str();
}

void Post::callFunctionByStatusCode()
{
	std::map<int, void (Post::*) (void)>::iterator it = fun_map.find(client_ref.statuc_code);
	if (it == fun_map.end())
		return ;
	(this->*(it->second))();
}
