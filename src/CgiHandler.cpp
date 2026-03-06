
#include "../hpp/CgiHandler.hpp"
#include <sys/types.h>
#include <sys/stat.h>
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

// Unchunk Transfer-Encoding: chunked body
std::string CgiHandler::unchunkReq(std::string body) {
    std::string out;
    size_t pos = 0;
    while (true) {
        // Read chunk size line (hex)
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
            // skip trailer (if any), ends at "\r\n"
            size_t end = body.find("\r\n", pos);
            (void)end;
            break;
        }
        if (pos + chunk_size > body.size()) break; // malformed
        out.append(body, pos, chunk_size);
        pos += chunk_size;
        // skip CRLF after chunk
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

    // Basic CGI spec variables
    const std::string script_path = client_ref.best_mach.empty() ? client_ref.request["path"] : client_ref.best_mach;
    std::string query = client_ref.question_mark;
    if (query.empty()) {
        // Derive from request path if present: "/path?query"
        std::map<std::string, std::string>::iterator pit = client_ref.request.find("path");
        if (pit != client_ref.request.end()) {
            const std::string &p = pit->second;
            std::string::size_type qm = p.find('?');
            if (qm != std::string::npos) query = p.substr(qm + 1);
        }
    }

    // Common variables
    setEnvVar("REQUEST_METHOD", _method);
    setEnvVar("QUERY_STRING", query);
    setEnvVar("GATEWAY_INTERFACE", "CGI/1.1");
    setEnvVar("SERVER_PROTOCOL", client_ref.request.count("protocol") ? client_ref.request["protocol"] : "HTTP/1.1");
    setEnvVar("SERVER_SOFTWARE", "webserv/1.0");
    setEnvVar("SERVER_NAME", server_ref._server_name.empty() ? (client_ref.request.count("host") ? client_ref.request["host"] : "localhost") : server_ref._server_name);
    {
        std::ostringstream oss;
        oss << server_ref._port;
        setEnvVar("SERVER_PORT", oss.str());
    }
    setEnvVar("DOCUMENT_ROOT", server_ref._root);
    setEnvVar("SCRIPT_NAME", "/" + basename_from_path(script_path));
    setEnvVar("SCRIPT_FILENAME", script_path); // Needed for php-cgi
    setEnvVar("PATH_INFO", ""); // Optional; can be refined
    setEnvVar("REMOTE_ADDR", client_ref.request.count("remote_addr") ? client_ref.request["remote_addr"] : "127.0.0.1");

    // Content headers for POST
    std::string clen = client_ref.request.count("content-length") ? client_ref.request["content-length"] : "";
    std::string ctype = client_ref.request.count("content-type") ? client_ref.request["content-type"] : "";
    if (_method == "POST") {
        if (!clen.empty()) setEnvVar("CONTENT_LENGTH", clen);
        if (!ctype.empty()) setEnvVar("CONTENT_TYPE", ctype);
    }

    // PHP specific
    if (client_ref.cgi_type == "php") {
        // Many setups require REDIRECT_STATUS for php-cgi to emit headers properly
        setEnvVar("REDIRECT_STATUS", "200");
    }

    // If you need more vars, add here, e.g., HTTP_* headers mirrored
    for (std::map<std::string, std::string>::const_iterator it = client_ref.request.begin(); it != client_ref.request.end(); ++it) {
        // Map header names to HTTP_* (uppercase, '-'->'_')
        const std::string &k = it->first;
        const std::string &v = it->second;
        if (k.empty()) continue;
        // Skip ones we already set or non-header keys
        if (k == "path" || k == "protocol" || k == "method" || k == "host" || k == "remote_addr") continue;
        std::string canon = k;
        for (size_t i = 0; i < canon.size(); ++i) {
            char c = canon[i];
            if (c == '-') canon[i] = '_';
            else canon[i] = static_cast<char>(std::toupper(c));
        }
        // Prefix with HTTP_ for general headers
        setEnvVar(std::string("HTTP_") + canon, v);
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
    // Prepare argv
    std::vector<char*> argv;
    argv.push_back(const_cast<char*>(interp.c_str()));
    argv.push_back(const_cast<char*>(script.c_str()));
    argv.push_back(NULL);

    // Prepare envp
    std::vector<char*> envp;
    for (size_t i = 0; i < _envVec.size(); ++i) {
        envp.push_back(const_cast<char*>(_envVec[i].c_str()));
    }
    envp.push_back(NULL);

    // Create pipes
    int in_pipe[2];  // parent writes -> child reads (stdin)
    int out_pipe[2]; // child writes -> parent reads (stdout)
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
        // Child
        // Set up stdin/stdout
        dup2(in_pipe[0], STDIN_FILENO);
        dup2(out_pipe[1], STDOUT_FILENO);

        // Close unused fds
        close(in_pipe[1]);
        close(out_pipe[0]);
        close(in_pipe[0]);
        close(out_pipe[1]);

        // Change directory to script directory for relative paths
        std::string dir = dirname_from_path(script);
        if (chdir(dir.c_str()) == -1) {
            // If chdir fails, still attempt exec
        }

        // Execve
        execve(interp.c_str(), &argv[0], &envp[0]);
        // If execve fails
        std::string err = "Status: 500 Internal Server Error\r\nContent-Type: text/plain\r\n\r\nCGI exec failed: ";
        err += strerror(errno);
        write(STDOUT_FILENO, err.c_str(), err.size());
        _exit(127);
    }

    // Parent
    close(in_pipe[0]);  // parent will write to in_pipe[1]
    close(out_pipe[1]); // parent will read from out_pipe[0]

    // Prepare request body for CGI stdin (POST only)
    std::string body;
    if (client_ref.request.count("body")) {
        body = client_ref.request["body"];
    } else if (!client_ref.buffer.empty()) {
        // Some implementations store whole raw request in buffer; extract body if needed externally.
        // Here we fallback to buffer for simplicity if present.
        body = client_ref.buffer;
    }

    // Unchunk if needed
    if (client_ref.request.count("transfer-encoding")) {
        if (str_to_lower(client_ref.request["transfer-encoding"]).find("chunked") != std::string::npos) {
            body = unchunkReq(body);
        }
    }

    if (_method == "POST" && !body.empty()) {
        // Write body to child's stdin
        size_t written = 0;
        while (written < body.size()) {
            ssize_t w = write(in_pipe[1], body.c_str() + written, body.size() - written);
            if (w < 0) {
                if (errno == EINTR) continue;
                break;
            }
            written += static_cast<size_t>(w);
        }
    }
    // Close stdin after writing to signal EOF
    close(in_pipe[1]);

    // Read CGI output until EOF
    char buf[4096];
    while (true) {
        ssize_t r = read(out_pipe[0], buf, sizeof(buf));
        if (r < 0) {
            if (errno == EINTR) continue;
            break;
        }
        if (r == 0) break;
        _output.append(buf, buf + r);
    }
    close(out_pipe[0]);

    int status = 0;
    waitpid(pid, &status, 0);

    client_ref.cgibuf = _output;
    client_ref.statuc_code = 200;
    return 0;
}

void CgiHandler::cgi_run() {
    if (!is_method_allowed()) {
        client_ref.statuc_code = 405;
        client_ref.cgibuf.clear();
        return;
    }
    // Check script and interpreter existence
    if (!cgi_exist()) {
        client_ref.statuc_code = 404;
        client_ref.cgibuf.clear();
        return;
    }
    createEnvironment();
    execute();
}