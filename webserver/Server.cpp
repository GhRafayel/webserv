#include "Server.hpp"

Server::~Server() {
	for (size_t i = 0; i < _pollfds.size(); i++)
	{
		if (_pollfds[i].fd != -1)
		{
			close(_pollfds[i].fd);
		}
	}
}

Server::Server() : StringUtils(), _conf_file_path("conf/default.conf"), _conf(), _pollfds() {
	_conf_file_path = abs_Path(_conf_file_path);
	if(!_conf_file_path.size())
		throw std::runtime_error("Configuration file dose not exist or worng path!");
	call_member("initConfig");
}

Server::Server(const std::string & conf_file_path)
	: StringUtils(), _conf_file_path(conf_file_path),  _conf(), _pollfds()
{
	_conf_file_path = abs_Path(_conf_file_path);
	if(!_conf_file_path.size())
		throw std::runtime_error("Configuration file dose not exist or worng path!");
	call_member("initConfig");
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
			std::vector<Config> conf_temp(obj._conf);
			this->_conf = conf_temp;
			std::vector<pollfd> pollfd_temp(obj._pollfds);
			this->_pollfds = pollfd_temp;
		}
		return *this;
	}
	return *this;
}

void	Server::call_member(const std::string & fun_name)
{
	std::string fun_list[3] = {"initConfig", "create_server", "accept_loop"};

	void (Server::*fun_ref[3])(void) = { &Server::initConfig, &Server::create_server, &Server::accept_loop};
	try
	{
		for (size_t i = 0; i < fun_list->size(); i++)
		{
			if (fun_list[i] == fun_name)
			{
				(this->*fun_ref[i])();
				break;
			}
				
		}
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		throw;
	}
}

void	Server::start(){
	call_member("create_server");
	call_member("accept_loop");
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
	std::vector<std::string> a = split(str, "[end]", true);

	for (size_t i = 0; i < a.size(); i++)
	{
		Config conf(a[i]);
		if (conf.is_valid())
		{
			this->_conf.push_back(Config(conf));
			str.clear();
		}
	}
}

void	Server::create_server()
{
	int				_socket;
	
	for (size_t i = 0; i < _conf.size(); i++)
	{
		int 			opt = 1;
		struct pollfd	temp;
	
		temp.events = POLLIN;
		temp.revents = 0;
		_socket = socket(AF_INET, SOCK_STREAM, 0);

		if (_socket < 0)
			throw std::runtime_error("socket failed");

		if (fcntl(_socket, F_SETFL, O_NONBLOCK | FD_CLOEXEC) == -1)
			throw std::runtime_error("fcntl failed");

		if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
			throw std::runtime_error("setsockopt failed");

		if (bind(_socket, (struct sockaddr *)&_conf[i].addr, sizeof(_conf[i].addr)) == -1)
			throw std::runtime_error("bind failed");
		if (listen(_socket, 128) < 0)
			throw std::runtime_error("listen failed");
		temp.fd = _socket;
		_pollfds.push_back(temp);
	}
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
				// i = request(i);
				std::cout << "request" << std::endl;
			else if (_pollfds[i].revents & POLLOUT)
				//i = respons(i);
				std::cout << "respons" << std::endl;
			else
				i++;
			if (!g_running) break;
		}
		if (!g_running) break;
	}
}