#include "../hpp/CgiHandler.hpp"
#include <sys/types.h>
#include <sys/stat.h>
#include <cstdlib>
#include <sstream>
#include <signal.h>

CgiHandler::CgiHandler(Server & s_obj, Client & c_obj) : 
	_envMap(), _envVec(),
	server_ref(s_obj),
	client_ref(c_obj)
{}

CgiHandler::~CgiHandler() {}

void	CgiHandler::setEnvVar(const std::string & key, const std::string & val)
{
	_envMap.insert(std::make_pair(key, val));
}

std::string CgiHandler::dirname_from_path(const std::string & path)
{
	std::string::size_type pos = path.find_last_of("/\\");
	if (pos == std::string::npos)
		return std::string(".");
	if (pos == 0)
		return std::string("/");
	return path.substr(0, pos);
}

std::string CgiHandler::basename_from_path(const std::string & path)
{
	std::string::size_type pos = path.find_last_of("/\\");
	if (pos == std::string::npos)
		return path;
	return path.substr(pos + 1);
}

std::string CgiHandler::getEnvVar(std::string & key)
{
	std::map<std::string, std::string>::iterator it = _envMap.find(key);
	if (it == _envMap.end())
		return std::string();
	return it->second;
}

std::string CgiHandler::find_interpreter() {

	for(size_t i = 0; i < server_ref._cgi_paths.size(); i++)
	{
		std::string result = abs_Path(server_ref._cgi_paths[i] + client_ref.cgi_type);
		if (exists(result)) 
			return result;
	}
	return std::string();
}

bool	CgiHandler::is_method_allowed()
{
	bool s_method = server_ref.get_method(client_ref.method);
	bool l_method = false;

	if (client_ref.best_location_index != -1)
		l_method = server_ref._locations[client_ref.best_location_index].get_method(client_ref.method);
	
	if (l_method || s_method)
		return true;
	return false;
}

std::string CgiHandler::decodeQm(const std::string & str)
{
	std::string out;
	out.reserve(str.size());
	for (size_t i = 0; i < str.size(); i++)
	{
		if (str[i] == '%')
		{
			if (i + 2 < str.size())
			{
				char h1 = str[i + 1];
				char h2 = str[i + 2];
				int val = 0;
				if (std::isxdigit(h1) && std::isxdigit(h2))
				{
					if (h1 >= '0' && h1 <= '9')
						val = (h1 - '0') << 4;
					else
						val = ((std::toupper(h1) - 'A') + 10) << 4;
					if (h2 >= '0' && h2 <= '9')
						val |= (h2 - '0');
					else
						val |= (std::toupper(h2) - 'A') + 10;
					out.push_back(static_cast<char>(val));
					i += 2;
					continue;
				}
			}
			out.push_back('%');
		}
		else if (str[i] == '+')
			out.push_back(' ');
		else if (str[i])
			out.push_back(str[i]);
	}
	return out;
}

std::string	CgiHandler::unchunkReq(std::string & body)
{
	std::string	out;
	size_t		pos = 0;
	while (true)
	{
		size_t	line_end = body.find("\r\n", pos);
		if (line_end == std::string::npos)
			break;
		std::string size_str = body.substr(pos, line_end - pos);
		std::istringstream iss(size_str);
		std::ios::fmtflags f = iss.flags();
		iss.setf(std::ios::hex, std::ios::basefield);
		size_t	chunk_size = 0;
		iss >> chunk_size;
		iss.flags(f);
		pos = line_end + 2;
		if (chunk_size == 0)
		{
			body.find("\r\n", pos);
			break;
		}
		if (pos + chunk_size > body.size()) break;
		out.append(body, pos, chunk_size);
		pos += chunk_size;

		if (pos + 2 <= body.size() && body.compare(pos, 2, "\r\n") == 0)
			pos += 2;
		else
			break;
	}
	return out;
}

bool	CgiHandler::cgi_exist() {
	std::string script = abs_Path(client_ref.best_match);
	if (!exists(script) || client_ref.cgi_type.empty() || find_interpreter().empty()) 
		return false;
	return true;
}

void	CgiHandler::convertEnv() {
	_envVec.clear();
	for (std::map<std::string, std::string>::iterator it = _envMap.begin(); it != _envMap.end(); it++)
		_envVec.push_back(it->first + "=" + it->second);
}

void	CgiHandler::createEnvironment() {
	_envMap.clear();
	setEnvVar("REQUEST_METHOD", client_ref.method);
	setEnvVar("QUERY_STRING", client_ref.query);
	setEnvVar("GATEWAY_INTERFACE", "CGI/1.1");
	setEnvVar("SERVER_PROTOCOL", client_ref.request["protocol"]);
	setEnvVar("SERVER_SOFTWARE", "webserv/1.0");
	setEnvVar("SERVER_NAME", server_ref._server_name);
	setEnvVar("SERVER_PORT", int_to_string(server_ref._port));
	setEnvVar("DOCUMENT_ROOT", dirname_from_path(abs_Path(client_ref.best_match)));
	setEnvVar("SCRIPT_NAME", "/" + basename_from_path(client_ref.best_match));
	setEnvVar("SCRIPT_FILENAME", abs_Path(client_ref.best_match));
	setEnvVar("PATH_INFO", "");
	setEnvVar("REMOTE_ADDR", client_ref.request.count("remote_addr") ? client_ref.request["remote_addr"] : "127.0.0.1");
	
	if (client_ref.method == "POST")
	{
		std::string	clen = client_ref.request.count("content-length") ? client_ref.request["content-length"] : "";
		std::string ctype = client_ref.request.count("content-type") ? client_ref.request["content-type"] : "";
		if (!clen.empty())
				setEnvVar("CONTENT_LENGTH", clen);
		if (!ctype.empty())
			setEnvVar("CONTENT_TYPE", ctype);
	}
	if (client_ref.cgi_type == "php-cgi")
		setEnvVar("REDIRECT_STATUS", "200");
	
	std::map<std::string, std::string>::iterator it = client_ref.request.begin();
	while (it != client_ref.request.end())
	{
		if (it->first.empty() || it->first == "url_path" || it->first == "protocol"  || it->first == "method" || it->first == "Host" || it->first == "remote_addr")
		{
			it++;
			continue;
		}
		setEnvVar("HTTP_" + str_to_upper(it->first), it->second);
		it++;
	}
	convertEnv();
}

std::vector<char*> CgiHandler::init_argv() {
	std::vector<char*> out;
	out.push_back(const_cast<char*>(interp.c_str()));
	out.push_back(const_cast<char*>(script.c_str()));
	out.push_back(NULL);
	return out;
}

std::vector<char*> CgiHandler::init_envp() {
	std::vector<char*> out;
	for (size_t i = 0; i < _envMap.size(); i++)
		out.push_back(const_cast<char*>(_envVec[i].c_str()));
	out.push_back(NULL);
	return out;
}

void CgiHandler::child_process(std::vector<char*>& argv, std::vector<char*>& envp) {
		dup2(client_ref.in_pipe[0], STDIN_FILENO);
		dup2(client_ref.out_pipe[1], STDOUT_FILENO);
		close(client_ref.in_pipe[0]), close(client_ref.out_pipe[0]);
		close(client_ref.in_pipe[1]), close(client_ref.out_pipe[1]);
		execve(argv[0], &argv[0], &envp[0]);
		std::_Exit(1);
}

int	CgiHandler::execute() {
	std::vector<char*> argv = init_argv();
	std::vector<char*> envp = init_envp();

	if (pipe(client_ref.in_pipe) == -1)
		return (client_ref.status_code = 500, -1);
	if (pipe(client_ref.out_pipe) == -1)
	{
		close(client_ref.in_pipe[0]), close(client_ref.in_pipe[1]);
		return (client_ref.status_code = 500, -1);
	}

	client_ref.cgi_pid = fork();
	if (client_ref.cgi_pid < 0)
	{
		close(client_ref.in_pipe[0]), close(client_ref.in_pipe[1]);
		close(client_ref.out_pipe[0]), close(client_ref.out_pipe[1]);
		return (client_ref.status_code = 500, -1);
	}
	if (client_ref.cgi_pid == 0)
		child_process(argv, envp);

	fcntl(client_ref.out_pipe[0], F_SETFL, O_NONBLOCK);
	close(client_ref.in_pipe[0]), close(client_ref.out_pipe[1]);

	std::string body;
	if (client_ref.request.count("body"))
		body = client_ref.request["body"];
	else if (!client_ref.buffer.empty())
		body = client_ref.buffer;
	
	if (client_ref.request.count("transfer-encoding") && str_to_lower(client_ref.request["transfer-encoding"]).find("chunked") != std::string::npos)
		body = unchunkReq(body);
	if (client_ref.method == "POST" && !body.empty())
	{
		size_t	written = 0;
		while (written < body.size())
		{
			ssize_t	w = write(client_ref.in_pipe[1], body.c_str() + written, body.size() - written);
			if (w < 0) break;
			written += static_cast<size_t>(w);
		}
	}
	
	close(client_ref.in_pipe[1]);
	int status = 0;
	waitpid(client_ref.cgi_pid, &status, WNOHANG);
	return 0;
}

int	CgiHandler::cgi_run()
{
	if (!is_method_allowed())
		return (client_ref.status_code = 405, 1);
	if (!cgi_exist())
		return (client_ref.status_code = 404, 1);
	createEnvironment();

	interp = find_interpreter();
	if (interp.empty())
		return (client_ref.status_code = 500, 0);
	script = abs_Path(client_ref.best_match);

	execute();
	client_ref.cgi_run = true;
	return 0;
}

