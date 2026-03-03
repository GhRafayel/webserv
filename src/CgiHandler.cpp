#include "../hpp/CgiHandler.hpp"

// TODO: cleanup CgiHandler!
// TODO: figure out, if we should whats up with .php and .py?!
// TODO: PATH_INFO?!?

CgiHandler::~CgiHandler() {}

CgiHandler::CgiHandler( Server & s_obj,  Client & c_obj) : Response(s_obj, c_obj)
{
	_method = client_ref.method;
	create_response();
}

void CgiHandler::createEnvironment() {
	std::string scriptPath = client_ref.best_mach;
	size_t qpos = scriptPath.find('?');
	if (qpos != std::string::npos) {
		scriptPath = scriptPath.substr(0, qpos);
	}
	std::string& req = client_ref.cgibuf;
	std::vector<std::string> vec;

	while (true) {
		size_t j = req.find("\r\n");
		if (j == std::string::npos || j == 0) {
			break;
		}
		std::string buf = req.substr(0, j);
		vec.push_back(buf);
		req = req.substr(j + 2);
	}

	std::string reqBody = req.substr(2);

	std::vector<std::string>::iterator it;
	for (it = vec.begin() + 1; it != vec.end(); ++it) {
		if (!(*it).compare(0, (*it).find(':'), "Content-Length")) {
			setEnvVar("CONTENT_LENGTH", (*it).substr((*it).find_first_of(' ') + 1));
		}
		else if (!(*it).compare(0, (*it).find(':'), "Content-Type")) {
			setEnvVar("CONTENT_TYPE", (*it).substr((*it).find_first_of(' ') + 1));
		}
		else {
			std::string key = "HTTP_" + (*it).substr(0, (*it).find(':'));
			for (size_t i = 0; i < key.size(); i++) {
				if (std::islower(key[i])) {
					key[i] -= 32;
				}
				else if (key[i] == '-') {
					key[i] = '_';
				}
			}
			setEnvVar(key, (*it).substr((*it).find_first_of(' ') + 1));
		}
	}
	if (!vec.empty()) {
		setEnvVar("REQUEST_METHOD", vec[0].substr(0, vec[0].find_first_of(' ')));
	}
	if (reqBody.size()) {
		if (_envMap.find("HTTP_TRANSFER_ENCODING") != _envMap.end()
			&& _envMap.find("HTTP_TRANSFER_ENCODING")->second == "chunked")
		{
			setEnvVar("REQUEST_BODY", unchunkReq(reqBody));
		}
		else {
			setEnvVar("REQUEST_BODY", reqBody);
		}
	}
	setEnvVar("QUERY_STRING", decodeQm(client_ref.question_mark));
	setEnvVar("REDIRECT_STATUS", "200");
	setEnvVar("SCRIPT_FILENAME", abs_Path(scriptPath));
}

bool	CgiHandler::cgi_exist()
{
	std::string pathOnly = client_ref.best_mach;
	size_t qpos = pathOnly.find('?');
	if (qpos != std::string::npos) {
		pathOnly = pathOnly.substr(0, qpos);
	}
	size_t post = pathOnly.rfind(".");
	if (post == std::string::npos)
		return false;
	bool val = false;

	for (size_t i = 0; client_ref.best_location_index != -1 && i < server_ref._locations[client_ref.best_location_index]._cgi.size(); i++)
	{
		if (pathOnly.substr(post) == server_ref._locations[client_ref.best_location_index]._cgi[i])
			val = true;
	}
	if (!val)
	{
		path = server_ref._error_404;
		ext = ".html";
		create_header(" 404 Not Found", true);
		return val;
	}
	return val;
}

void	CgiHandler::create_response() {
	
	if (!cgi_exist()) return ;
	createEnvironment();
	if (!is_method_allowed()) {
		create_header(" 405 Not Allowed", false);
		return;
	}
	if (!execute()) {
		strim << client_ref.request.find("protocol")->second << " 200 ok" << "\r\n";
		if (_output.find("\r\n\r\n") != std::string::npos) {
			strim << "Content-Length: " << _output.size() - _output.find("\r\n\r\n") - 4 << "\r\n";
		}
		else {
			strim << "Content-Length: " << _output.size() << "\r\n";
		}
		strim << "Server: my Server " << "\r\n";
		strim << "Date: " << get_http_date() << "\r\n";
		strim << "Connection: close" << "\r\n";
		strim << _output;
	}
	else {
		body = get_file_content("./www/errors/500.html");
		ext = ".html";
		create_header(" 500 Internal Server Error", false);
		strim << body;
	}
	client_ref.outbuf = strim.str();
}

void CgiHandler::convertEnv() {
	std::map<std::string, std::string>::iterator it;
	for (it = _envMap.begin(); it != _envMap.end(); ++it) {
		_envVec.push_back(it->first + '=' + it->second);
	}
}

int CgiHandler::execute() {
	std::map<std::string, std::string>::iterator it = _envMap.find("SCRIPT_FILENAME");
	
	if (it == _envMap.end()) return 1;

	char* argv[3];
	argv[0] = const_cast<char*>("/usr/bin/php-cgi");
	argv[1] = const_cast<char*>((*it).second.c_str());
	argv[2] = NULL;

	convertEnv();

	std::vector<char*> ptrs;
	ptrs.reserve(_envVec.size() + 1);
	for (size_t i = 0; i < _envVec.size(); i++) {
		ptrs.push_back(const_cast<char*>(_envVec[i].c_str()));
	}
	ptrs.push_back(NULL);
	char** envp = &ptrs[0];

	int in[2];
	int out[2];

	if (pipe(out) == -1) {
		return 1;
	}

	if (pipe(in) == -1)
	{
		close(out[0]);
		close(out[1]);
		return 1;
	}

	int pid = fork();

	if (pid == -1) {
		close(out[0]);
		close(out[1]);
		close(in[0]);
		close(in[1]);
		return 1;
	}
	if (pid == 0) {
		close(out[0]);
		dup2(out[1], STDOUT_FILENO);
		close(out[1]);

		close(in[1]);
		dup2(in[0], STDIN_FILENO);
		close(in[0]);
		execve(argv[0], argv, envp);
		std::exit(1);
	}
	else {
			close(out[1]);
			close(in[0]);
			std::map<std::string, std::string>::iterator body_it = _envMap.find("REQUEST_BODY");
			if (body_it != _envMap.end() && !body_it->second.empty()) {
				ssize_t written = write(in[1], body_it->second.c_str(), body_it->second.size());
				if (written == -1) {
					close(in[1]);
					close(out[0]);
					return 1;
				}
			}
			close(in[1]);
			_output.clear();
			char buffer[4096];
			ssize_t bread;

			while ((bread = read(out[0], buffer, sizeof(buffer) - 1)) > 0) {
				buffer[bread] = '\0';
				_output.append(buffer, bread);
				std::memset(buffer, 0, sizeof(buffer));
			}

			close(out[0]);
			int status;
			waitpid(pid, &status, 0);
			
			if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
				return 0;
			}
		}
	return 1;
}

void CgiHandler::setEnvVar(std::string key, std::string val) {
	_envMap[key] = val;
}

std::string CgiHandler::unchunkReq(std::string body) {
	std::string out;
	while (body.size() > 0) {
		char* end;
		long size = std::strtol(body.c_str(), &end, 16);
		if (size == 0 || *end != '\r' || size < 0) {
			break;
		}
		
		size_t header = (end - body.c_str()) + 2;
		if (body.size() < header + size + 2) {
			break;
		}

		out.append(body, header, size);
		body.erase(0, header + size + 2);
	}
	return out;
}

std::string CgiHandler::decodeQm(const std::string& qm) {
	std::string out;

	for(size_t i = 0; i < qm.size(); i++) {
		if (i + 2 < qm.size() && qm[i] == '%' && std::isxdigit(qm[i + 1]) && std::isxdigit(qm[i + 2]))
		{
			int c = std::strtol(qm.substr(i + 1, 2).c_str(), NULL, 16);
			out += c;
			i += 2;
		}
		else {
			out += qm[i];
		}
	}
	return out;
}
