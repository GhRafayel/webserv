#include "Config.hpp"

Config::~Config() {
	for (std::map<int, Server *>::iterator  it = _servers.begin(); it != _servers.end(); it++)
	{
		delete it->second;
	}
	for (size_t i = 0; i < _pollfds.size(); i++)
	{
		if (_pollfds[i].fd != -1)
			close(_pollfds[i].fd);
	}
}

Config::Config() : 
	StringUtils(),
	_client(), 
	_servers(), 
	_pollfds(),
	_time(1000),
	_conf_file_path("conf/default.conf")
{
	_conf_file_path = abs_Path(_conf_file_path);
	if(!_conf_file_path.size())
		throw std::runtime_error("Configuration file dose not exist or worng path!");
}

Config::Config(const std::string & conf_file_path) :
	StringUtils(),
	_client(),
	_servers(), 
	_pollfds(),
	_time(1000),
	_conf_file_path(conf_file_path)
{
	_conf_file_path = abs_Path(_conf_file_path);
	if(_conf_file_path.empty())
		throw std::runtime_error("Configuration file dose not exist or worng path!");
}

Config::Config(const Config & obj) :
	StringUtils(),
	_client(obj._client),
	_servers(obj._servers),
	_pollfds(obj._pollfds)
{
	this->_conf_file_path = obj._conf_file_path;
	this->_time = obj._time;
}

Config & Config::operator = (const Config & obj) {
	if (this != & obj)
	{
		this->_conf_file_path = obj._conf_file_path;
		for (size_t i = 0; i < obj._servers.size(); i++)
		{
			std::map<int, Server *> conf_temp(obj._servers);
			this->_servers.clear();
			this->_servers = conf_temp;
			std::vector<pollfd> pollfd_temp(obj._pollfds);
			this->_pollfds.clear();
			this->_pollfds = pollfd_temp;
		}
		return *this;
	}
	return *this;
}

pollfd	Config::create_pollfd(int fd)
{
	pollfd p;
	p.events = POLLIN;
	p.revents = 0;
	p.fd = fd;
	return p;
}

void	Config::start_server()
{
	initConfig();
	create_server();
	accept_loop();
}

void	Config::initConfig()
{
	validate_file(_conf_file_path);

	std::ifstream	file(_conf_file_path.c_str());
	std::string		buffer;
	std::string		str;

	while (getline(file, buffer))
	{
		buffer = trim(buffer, " \t");
		if (buffer[0] == '#' || !buffer.length())
			continue;
		str += buffer + "\n";
	}
	std::vector<std::string> Config = split(str, "[end]", true);
	for (size_t i = 0; i < Config.size(); i++)
	{
		Server conf(Config[i]);
		if (conf.is_valid())
		{
			int fd = socket(AF_INET, SOCK_STREAM, 0);
			if (fd < 0)
				throw std::runtime_error("socket failed\n");
			_servers.insert(std::make_pair(fd, new Server(conf)));
			str.clear();
		}
	}
}

void	Config::create_server()
{
	int				opt;

	for (std::map<int, Server *>::iterator it = _servers.begin(); it != _servers.end(); it++)
	{
		opt = 1;
		std::cout << "it->first = " << it->first << " it->second->addr.port = " << it->second->addr.sin_port << std::endl;
		if (fcntl(it->first, F_SETFL, O_NONBLOCK | FD_CLOEXEC) == -1)
            throw std::runtime_error("fcntl failed");
		if (setsockopt(it->first, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
			throw std::runtime_error("setsockopt failed\n");
		if (bind(it->first, (struct sockaddr *)&it->second->addr, sizeof(it->second->addr)) == -1)
			throw std::runtime_error("bind failed\n");
		if (listen(it->first, 128) < 0)
			throw std::runtime_error("listen failed\n");
		_pollfds.push_back(create_pollfd(it->first));
	}
}

void     Config::to_connect(int index)
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
		std::cout << "Client connected successfully" << std::endl;
	}
	else
		std::cout << "Client connection failed" << std::endl;
}

bool	Config::is_server_socket(int fd)
{
	std::map<int, Server *>::iterator it = _servers.find(fd);
	if (it != _servers.end())
		return true;
	return false;
}

void	Config::remove_client(int i)
{
	close(_pollfds[i].fd);
	_client.erase(_pollfds[i].fd);
	_pollfds.erase(_pollfds.begin() + i);
}

int	Config::to_read(Client & obj)
{
	char	buffer[1025];

	int n = recv(obj.fd, buffer, 1024, 0);
	if (n)
	{
		buffer[n] = '\0';
		obj.buffer.append(buffer);
		obj.end_request = end_of_request(buffer);
	}
	return n;
}


bool    Config::end_of_request(const std::string & buffer)
{
    if (buffer.length() < 4)
        return false;
    if (buffer.find("\r\n\r\n") != std::string::npos)
        return true;
    return false;
}

void    Config::accept_loop()
{
	std::cout << "\nServer start ...\n";

	while (g_running)
	{
		int n = poll(_pollfds.data(), _pollfds.size(), 1000);
		if (n <= 0) continue;

		size_t i = 0;
		while (g_running && i < _pollfds.size())
		{
			if ( _pollfds[i].revents & POLLIN)
			{
				if (is_server_socket(_pollfds[i].fd))
				{
					to_connect(i);
				}
				else
				{
					Client		&ref = _client.find(_pollfds[i].fd)->second;
			
					if (to_read(ref) == 0)
					{
						remove_client(i);
						break;
					}
					if (ref.end_request)
					{
						Request		req(ref);
						std::map<std::string, std::string> & map_ref = req.getRequest();

						std::cout << req.getMethod() << std::endl;
						std::cout << req.getPath() << std::endl;
						std::cout << req.getProtocol() << std::endl;

						for (std::map<std::string, std::string>::iterator it = map_ref.begin(); it != map_ref.end(); it++)
						{
							std::cout << it->first << " = " << it->second << std::endl;
						}
						
					}

					validate_file("index.html");
					std::ifstream file("index.html");
				
					std::string body;
					if (file.is_open())
					{
						std::ostringstream oss;
						oss << file.rdbuf();
						body = oss.str();
						file.close();
					}
					std::ostringstream t;
					t	<< "HTTP/1.1 200 OK\r\n"
						<< "Content-Type: text/html\r\n"
						<< "Connection: keep-alive\r\n"
						<< "Content-Length: " << body.size() <<  "\r\n"
						<< "\r\n" << body;
					_client.find(_pollfds[i].fd)->second.outbuf = t.str();
					_pollfds[i].events |= POLLOUT;
				}
			}
			else if ( _pollfds[i].revents & POLLOUT)
			{
				while (!_client.find(_pollfds[i].fd)->second.outbuf.empty())
				{
					_client.find(_pollfds[i].fd)->second.end_request = true;
					ssize_t sent = send(
						_pollfds[i].fd, 
						_client.find(_pollfds[i].fd)->second.outbuf.data(), 
						_client.find(_pollfds[i].fd)->second.outbuf.size(), 0);
					if (sent > 0)
					{
						_client.find(_pollfds[i].fd)->second.outbuf.erase(0, sent);
						continue;
					}
					if (sent == -1) break;
				}
				if (_client.find(_pollfds[i].fd)->second.end_request)
				{
					close(_pollfds[i].fd);
					_client.erase(_pollfds[i].fd);
					_pollfds.erase(_pollfds.begin() + i);
				}
				_pollfds[i].events &= ~POLLOUT;
			}
			i++;
		}
	}
}
