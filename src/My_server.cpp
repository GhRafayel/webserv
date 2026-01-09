#include "../hpp/My_server.hpp"

My_server::~My_server() {
	for (size_t i = 0; i < _pollfds.size(); i++)
	{
		if (_pollfds[i].fd != -1)
			close(_pollfds[i].fd);
	}
}

My_server::My_server() : StringUtils(),
	_env(NULL),
	_client(), 
	_servers(), 
	_pollfds(),
	_time(1000),
	_conf_file_path(abs_Path("conf/default.conf"))
{
	if(!_conf_file_path.empty())
		throw std::runtime_error("Configuration file dose not exist or worng path!");
}

My_server::My_server(char **env) : StringUtils(),
	_env(env),
	_client(),
	_servers(), 
	_pollfds(),
	_time(1000),
	_conf_file_path(abs_Path("conf/default.conf"))
{
	if(_conf_file_path.empty())
		throw std::runtime_error("Configuration file dose not exist or worng path!");
}

My_server::My_server(const std::string & conf_file_path,  char **env) :
	StringUtils(),
	_env(env),
	_client(),
	_servers(), 
	_pollfds(),
	_time(1000),
	_conf_file_path(abs_Path(conf_file_path))
	
{
	if(_conf_file_path.empty())
		throw std::runtime_error("Configuration file dose not exist or worng path!");
}

My_server::My_server(const My_server & obj) :StringUtils(),
	_env(obj._env),
	_client(obj._client),
	_servers(obj._servers),
	_pollfds(obj._pollfds)
{
	this->_conf_file_path = obj._conf_file_path;
	this->_time = obj._time;
}

My_server & My_server::operator = (const My_server & obj)
{
	if (this != & obj)
	{
		this->_client = obj._client;
		this->_servers = obj._servers;
		this->_pollfds = obj._pollfds;
		this->_conf_file_path = obj._conf_file_path;
		this->_time = obj._time;
		this->_env = obj._env;
	}
	return *this;
}

pollfd	My_server::create_pollfd(int fd)
{
	pollfd p;
	p.events = POLLIN;
	p.revents = 0;
	p.fd = fd;
	return p;
}

void	My_server::start_server()
{
	initConfig();
	for (std::map<int, Server>::iterator it = _servers.begin(); it != _servers.end(); it++)
	{
		try
		{
			create_server(it);
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what() << '\n';
		}
	}
	if (_pollfds.size())
		accept_loop();
	else
		std::cerr << "Error: No server available to start." << std::endl;
}

void	My_server::initConfig()
{
	if (!exists(_conf_file_path,'r'))
		throw std::runtime_error("conf path failed\n");

	ConfigPars config(_conf_file_path);
	
	for (size_t i = 0; i < config.servers.size(); i++)
	{
		int fd = socket(AF_INET, SOCK_STREAM, 0);

		if (fd < 0)
			throw std::runtime_error("socket failed\n");
		_servers.insert(std::make_pair(fd, Server(config.servers[i])));
	}
}

void	My_server::create_server(const std::map<int, Server>::iterator & it)
{
	int		opt = 1;

	if (fcntl(it->first, F_SETFL, O_NONBLOCK | FD_CLOEXEC) == -1)
		throw std::runtime_error("fcntl failed");
	if (setsockopt(it->first, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		throw std::runtime_error("setsockopt failed\n");
	if (bind(it->first, (struct sockaddr *)&it->second._addr, sizeof(it->second._addr)) == -1)
		throw std::runtime_error("bind failed\n" + int_to_string(ntohs(it->second._addr.sin_port)));
	if (listen(it->first, 128) < 0)
		throw std::runtime_error("listen failed\n");
	_pollfds.push_back(create_pollfd(it->first));
}

void     My_server::to_connect(int index)
{
	int fd = accept(_pollfds[index].fd, NULL, NULL);
	if (fd >= 0)
	{
		if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1)
		{
			close(fd);
			return;
		}
		_pollfds.push_back(create_pollfd(fd));
		_client.insert(std::make_pair(fd, Client(fd)));
		_client.find(fd)->second.server_conf_key = _pollfds[index].fd;
		std::cout << "Client connected successfully " << _pollfds.size() <<  std::endl;
	}
	else
		std::cout << "Client connection failed" << std::endl;
}

bool	My_server::is_server_socket(int fd)
{
	std::map<int, Server>::iterator it = _servers.find(fd);
	if (it != _servers.end())
		return true;
	return false;
}

void	My_server::remove_conection(int index)
{
	close(_pollfds[index].fd);
	_client.erase(_pollfds[index].fd);
	_pollfds.erase(_pollfds.begin() + index);
}

int	My_server::to_read(Client & obj)
{
	char	buffer[1025];

	int n = recv(obj.fd, buffer, 1024, 0);
	if (n > 0)
	{
		buffer[n] = '\0';
		obj.buffer.append(buffer);
		obj.end_request = is_end_of_request(buffer);
	}
	return n;
}


void	My_server::poll_in(int index)
{
	Client	&c_ref = _client.find(_pollfds[index].fd)->second;
	Server	&s_ref = _servers.find(c_ref.server_conf_key)->second;
	
	c_ref.timeOut = time(NULL);

	if (to_read(c_ref) == 0)
	{
		remove_conection(index);
		return;
	}
	if (!c_ref.end_request) return ;
	
	Request	req(s_ref, c_ref);
	_pollfds[index].events |= POLLOUT;
}

void	My_server::fun_405(Client & obj)
{
	std::ostringstream strim;

	strim << "HTTP/1.1 405 Not Allowed\r\n Content-Type: application/octet-stream\r\n"
		  << "Content-Length: 0 \r\nServer: my Server \r\n"
		  << "Date: " << get_http_date() << "\r\nConnection: close \r\n\r\n";
	obj.outbuf = strim.str();
	ssize_t n = 1;
	
	while (n > 0)
	{
		n = send(obj.fd, obj.outbuf.data(), obj.outbuf.size(), 0);
		if (n > 0)
			obj.outbuf.erase(0, n);
	}
}

Response * My_server::get_class(Server & s_obj, Client & c_obj)
{
	if (c_obj.method == "GET") {
		return (new Get(s_obj, c_obj));
	}
	else if (c_obj.method == "POST") {
		return (new Post(s_obj, c_obj));
	}
	else if (c_obj.method == "DELETE") {
		return (new Delete(s_obj, c_obj));
	}
	fun_405(c_obj);
	return NULL;
}

void	My_server::poll_out(int index)
{
	Client	&c_ref = _client.find(_pollfds[index].fd)->second;
	Server  &s_ref = _servers.find(c_ref.server_conf_key)->second;

	Response * response = get_class(s_ref, c_ref);
	if (response)
		response->send_response();
	if (c_ref.end_request && c_ref.outbuf.empty())
		remove_conection(index);
	delete response;
}

void My_server::time_out()
{
	for (size_t index = 0; index < _pollfds.size(); index++)
	{
		if(!is_server_socket(_pollfds[index].fd))
		{
			std::time_t corent_time = time(NULL);
			Client	&c_ref = _client.find(_pollfds[index].fd)->second;

			if (corent_time - c_ref.timeOut > 10 && !c_ref.end_request)
			{
				std::ostringstream strim;

				strim	<< "HTTP/1.1 408 Request Timeout \r\n"
						<< "Content-Length: 0 \r\n"
						<< get_my_taype("") << "\r\n"
						<< get_http_date() << "\r\n"
						<< "Connection: close \r\n"
						<< "\r\n" ;
				c_ref.end_request = true;
				c_ref.outbuf = strim.str();
				_pollfds[index].events |= POLLOUT;
			}
		}
	}
}

void    My_server::accept_loop()
{
	std::cout << "\nServer start ...\n";

	while (g_running && this->_pollfds.size())
	{
		int n = poll(_pollfds.data(), _pollfds.size(), 1000);

		if (n == 0) time_out();
			
		size_t i = 0;
		while (n > 0 && g_running && i < _pollfds.size())
		{
			if (_pollfds[i].revents & POLLIN)
			{
				if (is_server_socket(_pollfds[i].fd))
					to_connect(i);
				else
					poll_in(i);
			}
			else if (_pollfds[i].revents & POLLOUT)
				poll_out(i);
			i++;
		}
	}
}

