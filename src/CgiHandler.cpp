
#include "../hpp/CgiHandler.hpp"
CgiHandler::~CgiHandler() {}

CgiHandler::CgiHandler( Server & s_obj,  Client & c_obj) : Response(s_obj, c_obj)
{
	create_response();
}

void CgiHandler::createEnvironment() {
	std::stringstream req;
	req << client_ref.cgibuf;
	std::string buf;
	std::getline(req, buf);
	std::cout << buf;
}

void	CgiHandler::create_response() {
	createEnvironment();
	if (!is_method_allowed() || _method == "DELETE") {
		create_header(" 405 Not Allowed", false);
		return;
	}
	// else if (abs_Path(client_ref.best_mach) == "") {
	// 	std::cout << client_ref.best_mach << std::endl;
	// 	body = get_file_content("./www/errors/404.html");
	// 	create_header(" 404 Not Found", true);
	// 	return;
	// }
	if (!execute()) {
		_output = get_file_content("./www/whatever");
		std::remove("./www/whatever");
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
		create_header(" 500 Internal Server Error", true);
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

	if (it == _envMap.end()) {
		return 1;
	}
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



	int pid;
	pid = fork();
	if (pid == -1) {
		throw;
	}
	if (!pid) {
		int fd = open("./www/whatever", O_WRONLY | O_CREAT, 0777);
		if (fd == -1)
			std::exit(1);
		dup2(fd, STDOUT_FILENO);
		close(fd);
		execve(argv[0], argv, envp);
		std::exit(1);
	}
	else {
		int status;
		wait(&status);
		std::cout << "CGI-Status: " << WEXITSTATUS(status) << std::endl;
	}
	return (0);
}

void CgiHandler::setEnvVar(std::string key, std::string val) {
	_envMap.insert(std::pair<std::string, std::string>(key, val));
}

// TODO: delete this hilarious debug method
std::string CgiHandler::getEnvVar(std::string key) {
	std::map<std::string, std::string>::const_iterator it;
	for (it = _envMap.begin(); it != _envMap.end(); ++it) {
		if (it->first == key) {
			return it->first + "=" + it->second;
		}
	}
	return "help me pls";
}
