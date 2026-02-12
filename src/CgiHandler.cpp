
#include "../hpp/CgiHandler.hpp"
CgiHandler::~CgiHandler() {}

CgiHandler::CgiHandler( Server & s_obj,  Client & c_obj) : Response(s_obj, c_obj)
{
	create_response();
}

void CgiHandler::createEnvironment() {
	std::string& req = client_ref.cgibuf;
	std::vector<std::string> vec;

	while (true) {
		size_t j = req.find("\r\n");
		if (j == std::string::npos || req.find("\r\n") == req.find_last_of('\r'))
			break;
		std::string buf = req.substr(0, j);
		vec.push_back(buf);
		req = req.substr(j + 2);
	}

	std::string reqBody = req.substr(2);

	std::vector<std::string>::iterator it;
	for (it = vec.begin() + 1; it != vec.end(); ++it) {
		if (!(*it).compare(0, (*it).find(':'), "Content-Length")) {
			std::cout << "found content length" << std::endl;
			setEnvVar("CONTENT_LENGTH", (*it).substr((*it).find_first_of(' ') + 1));
		}
		else if (!(*it).compare(0, (*it).find(':'), "Content-Type")) {
			std::cout << "found content type" << std::endl;
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
	setEnvVar("REQUEST_METHOD", vec[0].substr(0, vec[0].find_first_of(' ')));
	if (reqBody.size()) {
		if (_envMap.find("HTTP_TRANSFER_ENCODING")->second == "chunked") {
			setEnvVar("REQUEST_BODY", unchunkReq(reqBody));
		}
		else {
			setEnvVar("REQUEST_BODY", reqBody);
		}
	}
	setEnvVar("QUERY_STRING", decodeQm(client_ref.question_mark));
	setEnvVar("REDIRECT_STATUS", "200");
	setEnvVar("SCRIPT_FILENAME", abs_Path("www" + client_ref.request.find("url_path")->second));
}

void	CgiHandler::create_response() {
	createEnvironment();
	if (!is_method_allowed() || _method == "DELETE") {
		create_header(" 405 Not Allowed", false);
		return;
	}
	if (!execute()) {
		strim << client_ref.request.find("protocol")->second << " 200 ok" << "\r\n";
		strim << "Content_Length: " << _output.size() - _output.find("\r\n") - 4 << "\r\n";
		strim << "Server: my Server " << "\r\n";
		strim << "Data: " << get_http_date() << "\r\n";
		strim << "Connection: close" << "\r\n";
		std::cout << strim.str() << std::endl;
		strim << _output;
	}
	else {
		body = get_file_content("./www/errors/500.html");
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

	// TODO: implement WORKING!!!! pipes
		// redir into child REQUEST_BODY -> second;
	int in[2];
	int out[2];

	pipe(out);
	if (_method == "POST") {
		pipe(in);
	}

	int pid;
	pid = fork();
	if (pid == -1) {
		throw std::runtime_error("fork failed");
	}
	if (!pid) {
		dup2(out[1], STDOUT_FILENO);
		close(out[1]);
		close(out[0]);

		if (_method == "POST") {
			dup2(in[0], STDIN_FILENO);
			close(in[0]);
			close(in[1]);
		}
		execve(argv[0], argv, envp);
		std::exit(1);
	}
	else {
		close(out[1]);
		
		// TODO: write to in[1];
		if (_method == "POST") {
			close(in[0]);
			std::map<std::string, std::string>::iterator it = _envMap.find("REQUEST_BODY");
			if (it != _envMap.end()) {
				write(in[1], it->second.c_str(), it->second.size());
			}
			close(in[1]);
		}

		// TODO: read from out[0]
		int status = 0;
		std::cout << "CGI-Status: " << WEXITSTATUS(status) << std::endl;
		while (true) {
			char buffer[1025] = {0};
			ssize_t bread = read(out[0], &buffer, 1024);
			if (bread == 0) break;
			else if (bread == -1) {
				close(out[0]);
				return 1;
			}
			_output += buffer;
		}

		close(out[0]);
		waitpid(pid, &status, 0);

	}
	return (0);
}

void CgiHandler::setEnvVar(std::string key, std::string val) {
	_envMap.insert(std::pair<std::string, std::string>(key, val));
}

std::string CgiHandler::unchunkReq(std::string body) {
	std::string out;
	// TODO: find bug inside of here!!!
	while (body.size() > 4 && body != "\r\n") {
		int size = std::strtol(body.c_str(), NULL, 16);
		body = body.substr(body.find("\r\n") + 2);
		out += body.substr(0, size);
		body = body.substr(body.find("\r\n") + 2);
	}
	return out;
}

std::string CgiHandler::decodeQm(const std::string& qm) {
	std::string out;

	for(size_t i = 0; i < qm.size(); i++) {
		if (qm[i] == '%'
			&& (std::isdigit(qm[i + 1]) || (qm[i + 1] >= 'A' && qm[i + 1] <= 'F'))
			&& (std::isdigit(qm[i + 2]) || (qm[i + 2] >= 'A' && qm[i + 2] <= 'F')))
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
