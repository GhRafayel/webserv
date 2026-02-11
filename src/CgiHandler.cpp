
#include "../hpp/CgiHandler.hpp"
CgiHandler::~CgiHandler() {}

CgiHandler::CgiHandler( Server & s_obj,  Client & c_obj) : Response(s_obj, c_obj)
{
	_envMap.insert(std::pair<std::string, std::string>("REQUEST_METHOD", ""));					// GET,POST,DELETE,...
	_envMap.insert(std::pair<std::string, std::string>("SCRIPT_FILENAME", ""));					// full path to PHP file to execute (PHP needs this)
	_envMap.insert(std::pair<std::string, std::string>("REDIRECT_STATUS", "200"));				// set to "200"(PHP-CGI security requirement, won't run without it)

	_envMap.insert(std::pair<std::string, std::string>("CONTENT_LENGTH", ""));					// number of bytes in the POST BODY
	_envMap.insert(std::pair<std::string, std::string>("CONTENT_TYPE", ""));					// usually "applications/x-www-form-urlencoded" or "multipart/form-data"

	_envMap.insert(std::pair<std::string, std::string>("QUERY_STRING", ""));					// everything after the '?' in the URL

	_envMap.insert(std::pair<std::string, std::string>("SERVER_PROTOCOL", "HTTP/1.1"));			// HTTP/1.1
	_envMap.insert(std::pair<std::string, std::string>("SERVER_SOFTWARE", ""));					// your servername/version
	_envMap.insert(std::pair<std::string, std::string>("GATEWAY_INTERFACE", "CGI/1.1"));		// CGI/1.1
	_envMap.insert(std::pair<std::string, std::string>("SCRIPT_NAME", ""));						// the URI path to the script
	
	_envMap.insert(std::pair<std::string, std::string>("SERVER_NAME", ""));						// hostname
	_envMap.insert(std::pair<std::string, std::string>("SERVER_PORT", ""));						// port number
	_envMap.insert(std::pair<std::string, std::string>("REMOTE_ADDR", ""));	
	
	create_response();
}

void	CgiHandler::create_response() {
	std::cout << "CGI-Request method: " << client_ref.method << std::endl;
	if (!is_method_allowed()) {
		create_header(" 405 Not Allowed", false);
	}
	else {
		std::string msg;
		setEnvVar("SCRIPT_FILENAME", "./www/index.php");	// TODO: find correct path
		if (!execute()) {
			msg = " 200 ok";
		}
		else {
			msg = " 500 Internal Server Error";
		}
		strim << client_ref.request.find("protocol")->second << msg << "\r\n";
		// strim << type from body;
		strim << "Content_Length: " << body.size() << "\r\n";
		strim << "Server: my Server " << "\r\n";
		strim << "Data: " << get_http_date() << "\r\n";
		strim << "Connection: close" << "\r\n\r\n";
		std::cout << strim.str() << std::endl;
		if (msg == " 200 ok") {
			strim << body;
		}
		else {
			strim << get_file_content("./www/errors/500.html");
		}
		client_ref.outbuf = strim.str();
	}
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


	if (_method == "GET") {
		setEnvVar("QUERY_STRING", "...");
	}
	else {
		setEnvVar("CONTENT_LENGTH", "");	// ????
		setEnvVar("CONTENT_TYPE", "");		// ????
		// piping
	}

	std::vector<char*> ptrs;
	ptrs.reserve(_envVec.size() + 1);
	for (size_t i = 0; i < _envVec.size(); i++) {
		ptrs.push_back(const_cast<char*>(_envVec[i].c_str()));
	}
	ptrs.push_back(NULL);
	char** envp = &ptrs[0];

	convertEnv();
	int pipeout[2];
	pipe(pipeout);		// TODO: not safe yet;
	int pid;

	pid = fork();


	if (pid == -1) {
		throw;
	}
	if (!pid) {
		// TODO: redirections || piping
		dup2(pipeout[1], STDOUT_FILENO);	//	=======================================
		close(pipeout[0]);
		close(pipeout[1]);
		execve(argv[0], argv, envp);
		std::exit(1);
	}
	else {
		int status;
		close(pipeout[1]);
		waitpid(pid, &status, 0);
		char buf[10000] = {0};
		ssize_t bread = read(pipeout[0], &buf, 10000);
		if (bread == -1) {
			close(pipeout[0]);
			return 1;
		}
		_output += buf;
		close(pipeout[0]);
	}
	return (0);
}

void CgiHandler::setEnvVar(std::string key, std::string val) {
	std::map<std::string, std::string>::iterator it = _envMap.find(key);
	if (it != _envMap.end()) {
		it->second = val;
	}
	else {
		std::string newKey = "HTTP_" + key;

		// replace - with _
		for (size_t i = 0; i < newKey.length(); i++) {
			if (newKey[i] == '-') {
				newKey[i] = '_';
			}
		}

		// convert to uppercase
		std::transform(newKey.begin(), newKey.end(), newKey.begin(), (int(*)(int))std::toupper);
		std::pair<std::string, std::string> toInsert;
		toInsert.first = newKey;
		toInsert.second = val;
		_envMap.insert(toInsert);
	}
}

std::string CgiHandler::getEnvVar(std::string key) {
	std::map<std::string, std::string>::const_iterator it;
	for (it = _envMap.begin(); it != _envMap.end(); ++it) {
		if (it->first == key) {
			return it->first + "=" + it->second;
		}
	}
	return "help me pls";
}

const std::string& CgiHandler::getOutput() const {
	return _output;
}
