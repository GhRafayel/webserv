#include "My_server.hpp"

My_server::~My_server() {
	for (size_t i = 0; i < _fds.size(); i++)
		if (_fds[i].fd) close(_fds[i].fd);
}

My_server::My_server() :_timeout_ms(1000),  _port(8080), _socket(-1) {

	_s_addr.sin_family = AF_INET;
	_s_addr.sin_port = htons(this->_port);
	_s_addr.sin_addr.s_addr = INADDR_ANY;
}

void My_server::create_socket() {
	int opt = 1;
	_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (_socket < 0)
		throw std::string("socket failed");
	if (fcntl(_socket, F_SETFL, O_NONBLOCK | FD_CLOEXEC) == -1)
		throw std::string("fcntl failed");
	if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    	throw std::string("setsockopt failed");
	if (bind(_socket, (struct sockaddr *)&_s_addr, sizeof(_s_addr)) == -1)
		throw std::string("bind failed");
	if (listen(_socket, 128) < 0)
		throw std::string("listen failed");
	_fds.push_back(create_pollfd(_socket));
}

pollfd	My_server::create_pollfd(int fd)
{
	pollfd p;
	p.events = POLLIN;
	p.fd = fd;
	p.revents = 0;
	return p;
}

void	My_server::remove_item(int index)
{
	_fds.erase(_fds.begin() + index);
	_client.erase(_client.begin() + index - 1);
}

void	My_server::add_item(int fd)
{
	_fds.push_back(create_pollfd(fd));
	_client.push_back(client(fd));
}

void My_server::start()
{
	create_socket();
	accept_loop();
}

void My_server::setPort(const int new_port) { this->_port = new_port; }

int	My_server::request(int index)
{
	std::string buffer;
	size_t		n;
	if (index == 0)
	{
		//std::cout << "Client connected " << std::endl;
		//int client_fd = accept(_socket, NULL, NULL);
		int client_fd = _req.connect(_socket);
		if (client_fd >= 0)
		{
			// if (fcntl(client_fd, F_SETFL, O_NONBLOCK) == -1)
			// 	throw std::string("fcntl failed");
			_req.nonBlock(client_fd);
			add_item(client_fd);
		}
		return index + 1;
	}

	buffer = _req.to_read(_fds[index].fd);
	n = buffer.length();
	// char buffer[1025];
	// int n = read(_fds[index].fd, buffer, 1024);

	if (n <= 0)
	{
		if (n == -1 &&  (errno != EAGAIN && errno != EWOULDBLOCK))
				return index + 1;
		close(_fds[index].fd);
		remove_item(index);
		return index;
	}
	//buffer[n] = '\0';
	_client[index - 1].buffer.append(buffer);
	if (!Pars::end_req(_client[index - 1].buffer))
		return index + 1;

	//std::cout << "[RAW] " << _client[index - 1].buffer << std::endl;

	std::vector<std::string > temp = Pars::split(_client[index - 1].buffer, "\r\n");

	for (size_t i = 0; i < temp.size(); i++)
	{
		std::cout << temp[i] << " === " << std::endl;
	}
	
	std::ifstream file("conf/index.html");
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
	_client[index - 1].outbuf = t.str();
	_fds[index].events |= POLLOUT;
	return index + 1;
}

int My_server::respons(int index)
{
	int fd = _fds[index].fd;
	client & c = _client[index - 1];

	while (!c.outbuf.empty())
	{
		c.val = true;
		ssize_t sent = send(fd, c.outbuf.data(), c.outbuf.size(), 0);
		if (sent > 0)
		{
			c.outbuf.erase(0, sent);
			continue;
		}
		if (sent == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))  return index + 1;
		return index;
	}
	if (c.val)
	{
		close(fd);
		remove_item(index);
	}
	_fds[index].events &= ~POLLOUT;
	return index;
}

bool    My_server::accept_loop()
{
	std::cout << "\nServer start ...\n";
	while (g_running)
	{
		int n = poll(_fds.data(), _fds.size(), _timeout_ms);

		if (n <= 0)	continue;

		size_t i = 0;
		while (i < _fds.size())
		{
			if (_fds[i].revents & POLLIN)
			{
				i = request(i);
			}
			else if (_fds[i].revents & POLLOUT)
			{
				i = respons(i);
			}
			else
				i++;
			if (!g_running) break;
		}
		if (!g_running) break;
	}
	return true;
}

