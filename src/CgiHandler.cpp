#include "../hpp/CgiHandler.hpp"
#include <sys/types.h>
#include <sys/stat.h>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <sstream>

CgiHandler::CgiHandler(Server & s_obj, Client & obj)
: _method(obj.method),
_envMap(),
_envVec(),
_output(),
server_ref(s_obj),
client_ref(obj)
{}

CgiHandler::~CgiHandler() {}

void CgiHandler::setEnvVar(std::string k, std::string v) {
	_envMap[k] = v;
}

std::string CgiHandler::dirname_from_path(const std::string &path) {
	std::string::size_type pos = path.find_last_of("/\\");
	if (pos == std::string::npos) return std::string(".");
	if (pos == 0) return std::string("/");
	return path.substr(0, pos);
}

std::string CgiHandler::basename_from_path(const std::string &path) {
	std::string::size_type pos = path.find_last_of("/\\");
	if (pos == std::string::npos) return path;
	return path.substr(pos + 1);
}

std::string CgiHandler::getEnvVar(std::string k) {
	std::map<std::string, std::string>::iterator it = _envMap.find(k);
	if (it == _envMap.end()) return std::string();
	return it->second;
}

std::string	CgiHandler::find_interpreter() {

	size_t dot = client_ref.best_mach.rfind(".");
	if (dot == std::string::npos) return "";

	std::string ext = str_to_lower(client_ref.best_mach.substr(dot + 1));

	if (ext == "php")
		ext = "php-cgi";
	else if (ext == "py")
		ext = "python3";
	else
		return "";
	
	for (size_t i = 0; i < server_ref._cgi_paths.size(); i++)
	{
		std::string result = abs_Path(server_ref._cgi_paths[i] + ext);
		
		if (exists(result)) 
			return result;
	}
	return "";
}

void CgiHandler::check_status_code() {

	if (_output.compare(0, 7, "Status:") == 0)
	{
		client_ref.statuc_code = std::atoi(_output.substr(7).c_str());
		return ;
	}
	client_ref.statuc_code = 200;
	client_ref.cgibuf = _output;
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

// Percent-decode utility: decodes %XX and '+' to ' ' like typical query-string.
std::string CgiHandler::decodeQm(const std::string &s) {
	std::string out;
	out.reserve(s.size());
	for (size_t i = 0; i < s.size(); ++i) {
		if (s[i] == '%') {
			if (i + 2 < s.size()) {
				char h1 = s[i+1];
				char h2 = s[i+2];
				int v = 0;
				if (std::isxdigit(h1) && std::isxdigit(h2)) {
					if (h1 >= '0' && h1 <= '9') v = (h1 - '0') << 4;
					else v = ((std::toupper(h1) - 'A') + 10) << 4;
					if (h2 >= '0' && h2 <= '9') v |= (h2 - '0');
					else v |= (std::toupper(h2) - 'A') + 10;
					out.push_back(static_cast<char>(v));
					i += 2;
					continue;
				}
			}
			// If malformed, keep '%'
			out.push_back('%');
		} else if (s[i] == '+') {
			out.push_back(' ');
		} else {
			out.push_back(s[i]);
		}
	}
	return out;
}

std::string CgiHandler::unchunkReq(std::string body) {
	std::string out;
	size_t pos = 0;
	while (true) {

		size_t line_end = body.find("\r\n", pos);
		if (line_end == std::string::npos) break;
		std::string size_str = body.substr(pos, line_end - pos);
		std::istringstream iss(size_str);
		std::ios::fmtflags f = iss.flags();
		iss.setf(std::ios::hex, std::ios::basefield);
		size_t chunk_size = 0;
		iss >> chunk_size;
		iss.flags(f);
		pos = line_end + 2;
		if (chunk_size == 0) {

			size_t end = body.find("\r\n", pos);
			(void)end;
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

bool CgiHandler::cgi_exist() {
	
	std::string script = abs_Path(client_ref.best_mach);
	if (client_ref.cgi_type.empty()) return false;
	if (!exists(script)) return false;
	if (find_interpreter().empty()) return false;
	return true;
}

void CgiHandler::convertEnv() {
	_envVec.clear();
	for (std::map<std::string, std::string>::iterator it = _envMap.begin(); it != _envMap.end(); ++it) {
		std::string kv = it->first + "=" + it->second;
		_envVec.push_back(kv);
	}
}

void CgiHandler::createEnvironment() {
	_envMap.clear();

	const std::string script_path = client_ref.best_mach.empty() ? client_ref.request["url_path"] : client_ref.best_mach;
	setEnvVar("REQUEST_METHOD", client_ref.method);
	setEnvVar("QUERY_STRING", client_ref.query);
	setEnvVar("GATEWAY_INTERFACE", "CGI/1.1");
	setEnvVar("SERVER_PROTOCOL", client_ref.request.count("protocol") ? client_ref.request["protocol"] : "HTTP/1.0");
	setEnvVar("SERVER_SOFTWARE", "webserv/1.0");
	setEnvVar("SERVER_NAME", server_ref._server_name.empty() ? "localhost" : server_ref._server_name);
	setEnvVar("SERVER_PORT", int_to_string(server_ref._port));
	setEnvVar("DOCUMENT_ROOT", dirname_from_path(abs_Path(client_ref.best_mach)));
	setEnvVar("SCRIPT_NAME", "/" + basename_from_path(script_path));
	setEnvVar("SCRIPT_FILENAME", abs_Path(script_path));
	setEnvVar("PATH_INFO", "");
	setEnvVar("REMOTE_ADDR", client_ref.request .count("remote_addr") ? client_ref.request["remote_addr"] : "127.0.0.1");
	if (client_ref.method == "POST") {
		std::string clen = client_ref.request.count("content-length") ? client_ref.request["content-length"] : "";
		std::string ctype = client_ref.request.count("content-type") ? client_ref.request["content-type"] : "";
		if (!clen.empty()) setEnvVar("CONTENT_LENGTH", clen);
		if (!ctype.empty()) setEnvVar("CONTENT_TYPE", ctype);
	}
	if (client_ref.cgi_type == "php")  setEnvVar("REDIRECT_STATUS", "200");
		for (std::map<std::string, std::string>::const_iterator it = client_ref.request.begin(); it != client_ref.request.end(); ++it) {
		const std::string &k = it->first;
		const std::string &v = it->second;
		if (k.empty()) continue;

		if (k == "url_path" || k == "protocol" || k == "method" || k == "Host" || k == "remote_addr") continue;
		setEnvVar(std::string("HTTP_") + str_to_upper(k), v);
	}

	convertEnv();
}

int CgiHandler::execute() {

	_output.clear();
	std::string interp = find_interpreter();
	if (interp.empty()) {
		client_ref.statuc_code = 500;
		return -1;
	}
	std::string script = abs_Path(client_ref.best_mach);
	std::vector<char*> argv;
	argv.push_back(const_cast<char*>(interp.c_str()));
	argv.push_back(const_cast<char*>(script.c_str()));
	argv.push_back(NULL);

	std::vector<char*> envp;
	for (size_t i = 0; i < _envVec.size(); ++i) {
		envp.push_back(const_cast<char*>(_envVec[i].c_str()));
	}
	envp.push_back(NULL);

	int in_pipe[2];
	int out_pipe[2];
	if (pipe(in_pipe) == -1) {
		client_ref.statuc_code = 500;
		return -1;
	}
	if (pipe(out_pipe) == -1) {
		close(in_pipe[0]); close(in_pipe[1]);
		client_ref.statuc_code = 500;
		return -1;
	}
	pid_t pid = fork();
	if (pid < 0) {
		close(in_pipe[0]); close(in_pipe[1]);
		close(out_pipe[0]); close(out_pipe[1]);
		client_ref.statuc_code = 500;
		return -1;
	}

	if (pid == 0) {

		dup2(in_pipe[0], STDIN_FILENO);
		dup2(out_pipe[1], STDOUT_FILENO);
		close(in_pipe[1]);
		close(out_pipe[0]);
		close(in_pipe[0]);
		close(out_pipe[1]);
		std::string dir = dirname_from_path(script);
		if (chdir(dir.c_str()) == -1) {
		}
		execve(interp.c_str(), &argv[0], &envp[0]);
		std::string err = "Status: 500 Internal Server Error\r\nContent-Type: text/plain\r\n\r\nCGI exec failed: ";
		err += strerror(errno);
		write(STDOUT_FILENO, err.c_str(), err.size());
		_exit(127);
	}

	close(in_pipe[0]);
	close(out_pipe[1]);

	std::string body;
	if (client_ref.request.count("body")) {
		body = client_ref.request["body"];
	} else if (!client_ref.buffer.empty()) {
		body = client_ref.buffer;
	}

	if (client_ref.request.count("transfer-encoding")) {
		if (str_to_lower(client_ref.request["transfer-encoding"]).find("chunked") != std::string::npos) {
			body = unchunkReq(body);
		}
	}

	if (_method == "POST" && !body.empty()) {
		size_t written = 0;
		while (written < body.size()) {
			ssize_t w = write(in_pipe[1], body.c_str() + written, body.size() - written);
			if (w < 0) {
				break;
			}
			written += static_cast<size_t>(w);
		}
	}

	close(in_pipe[1]);

	char buf[4096];
	while (true) {
		ssize_t r = read(out_pipe[0], buf, sizeof(buf));
		if (r <= 0) break;
		_output.append(buf, buf + r);
	}
	close(out_pipe[0]);
	int status = 0;

	waitpid(pid, &status, 0);
	check_status_code();
	return 0;
}

void CgiHandler::cgi_run() {
	if (!is_method_allowed()) {
		client_ref.statuc_code = 405;
		client_ref.cgibuf.clear();
		return;
	}

	if (!cgi_exist()) {
		client_ref.statuc_code = 404;
		client_ref.cgibuf.clear();
		return;
	}
	createEnvironment();
	execute();
}