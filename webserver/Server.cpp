#include "Server.hpp"

Server::~Server() {
	for (size_t i = 0; i < _conf.size(); i++)
		delete _conf[i];
	for (size_t i = 0; i < _pollfds.size(); i++)
	{
		if (_pollfds[i].fd != -1)
			close(_pollfds[i].fd);
	}
}

Server::Server()
	: StringUtils(), _sockets(), _client(), _conf_file_path("conf/default.conf"), _conf(), _pollfds() {
	_conf_file_path = abs_Path(_conf_file_path);
	if(!_conf_file_path.size())
		throw std::runtime_error("Configuration file dose not exist or worng path!");
}

Server::Server(const std::string & conf_file_path)
	: StringUtils(), _conf_file_path(conf_file_path),  _conf(), _pollfds()
{
	_conf_file_path = abs_Path(_conf_file_path);
	if(!_conf_file_path.size())
		throw std::runtime_error("Configuration file dose not exist or worng path!");
}

Server::Server(const Server & obj) : StringUtils(), _conf(obj._conf), _pollfds(obj._pollfds)
{
	this->_conf_file_path = obj._conf_file_path;
}

Server & Server::operator = (const Server & obj) {
	if (this != & obj)
	{
		this->_conf_file_path = obj._conf_file_path;
		for (size_t i = 0; i < obj._conf.size(); i++)
		{
			std::vector<Config *> conf_temp(obj._conf);
			this->_conf = conf_temp;
			std::vector<pollfd> pollfd_temp(obj._pollfds);
			this->_pollfds = pollfd_temp;
		}
		return *this;
	}
	return *this;
}

pollfd	Server::create_pollfd(int fd)
{
	pollfd p;
	p.events = POLLIN;
	p.fd = fd;
	p.revents = 0;
	return p;
}

void	Server::start()
{
	initConfig();
	create_server();
	// accept_loop();
}

void	Server::initConfig()
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
	std::vector<std::string> server = split(str, "[end]", true);
	for (size_t i = 0; i < server.size(); i++)
	{
		Config conf(server[i]);
		if (conf.is_valid())
		{
			this->_conf.push_back(new Config(conf));
			str.clear();
		}
	}
}

void	Server::create_server()
{
	int				fd;
	int				opt;
	for (size_t i = 0; i < _conf.size(); i++)
	{
		opt = 1;

		fd = socket(AF_INET, SOCK_STREAM, 0);

		if (fd < 0)
			throw std::runtime_error("socket failed");

		if (fcntl(fd, F_SETFL, O_NONBLOCK | FD_CLOEXEC) == -1)
			throw std::runtime_error("fcntl failed");

		if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
			throw std::runtime_error("setsockopt failed");

		if (bind(fd, (struct sockaddr *)&_conf[i]->addr, sizeof(_conf[i]->addr)) == -1)
			throw std::runtime_error("bind failed");
		if (listen(fd, 128) < 0)
			throw std::runtime_error("listen failed");
		_sockets.push_back(fd);
		_pollfds.push_back(create_pollfd(fd));
	}
}

void     Server::to_connect(int index)
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
		_client.insert(std::make_pair(index, Client(fd)));
		std::cout << "Client connected successfully" << std::endl;
	}
	else
	{
		std::cout << "Client connection failed" << std::endl;
		close(fd);
	}
}

bool	Server::is_server_socket(int fd)
{
	for (size_t i = 0; i < _sockets.size(); i++)
	{
		if (fd == _sockets[i])
			return true;
	}
	return false;
}

void    Server::accept_loop()
{
	std::cout << "\nServer start ...\n";

	while (g_running)
	{
		int n = poll(_pollfds.data(), _pollfds.size(), _time);

		if (n <= 0)	continue;

		size_t i = 0;
		while (i < _pollfds.size())
		{
			if (_pollfds[i].revents & POLLIN)
			{
				if (is_server_socket(_pollfds[i].fd))
				{
					to_connect(i);
				}
				else
				{
					Request(_client.find(i));
					//if (n == -1 &&  (errno != EAGAIN && errno != EWOULDBLOCK))
				}
				std::cout << "request" << std::endl;
			}
			else if (_pollfds[i].revents & POLLOUT)
			{
				// Respons(_client.find(i));
				// i++;
				std::cout << "respons" << std::endl;
			}
			else
				i++;
			if (!g_running) break;
		}
		if (!g_running) break;
	}
}