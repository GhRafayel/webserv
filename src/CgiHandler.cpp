
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
	// for (size_t i = 0; i < vec.size(); i++) {
	// 	std::cout << vec[i] << std::endl;
	// }

	std::string reqBody = req.substr(2);

	//	build environment with necessary headers and HTTP_ headers
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

	//	set vital meta variables
	// GET /index.php?...%20... HTTP/1.1
	setEnvVar("REQUEST_METHOD", vec[0].substr(0, vec[0].find_first_of(' ')));

	// TODO: write unchunk function
	if (reqBody.size()) {
		setEnvVar("REQUEST_BODY", reqBody);
		// setEnvVar("REQUEST_BODY", unchunkReq(reqBody));
	}
	
	// TODO: write decode function
	setEnvVar("QUERY_STRING", client_ref.question_mark);
	// setEnvVar("QUERY_STRING", decodeQm(client_ref.question_mark));

	setEnvVar("REDIRECT_STATUS", "200");
	setEnvVar("SCRIPT_FILENAME", abs_Path("www" + client_ref.request.find("url_path")->second));


	// TODO: delete this
	// std::cout << "======================" << std::endl;
	// for (std::map<std::string, std::string>::iterator it = _envMap.begin(); it != _envMap.end(); ++it) {
	// 	std::cout << it->first << "=" << it->second << std::endl;
	// }
}

void	CgiHandler::create_response() {
	std::cout << "======================" << std::endl;	// TODO: delete this
	createEnvironment();
	std::cout << "======================" << std::endl; // TODO: delete this
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
		create_header(" 500 Internal Server Error", false);
		strim << body;
	}
	client_ref.outbuf = strim.str();
	// std::cout << client_ref.outbuf << std::endl;
}

void CgiHandler::convertEnv() {
	std::map<std::string, std::string>::iterator it;
	for (it = _envMap.begin(); it != _envMap.end(); ++it) {
		_envVec.push_back(it->first + '=' + it->second);
	}
}

int CgiHandler::execute() {
	std::map<std::string, std::string>::iterator it = _envMap.find("SCRIPT_FILENAME");

	if (it == _envMap.end() || it->second.length() == 0) {
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

	// TODO: implement WORKING!!!! pipes
		// redir into child REQUEST_BODY -> second;


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
