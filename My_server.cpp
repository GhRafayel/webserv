#include "My_server.hpp"


My_server::~My_server() {
	for (size_t i = 0; i < _pollfds.size(); i++)
		if (_pollfds[i].fd) close(_pollfds[i].fd);
}

My_server::My_server() : _timeout_ms(1000),  _port(8080), _s_socket(-1) {}


void My_server::init() {

	_s_addr.sin_family = AF_INET;
	_s_addr.sin_port = htons(this->_port);
	_s_addr.sin_addr.s_addr = INADDR_ANY;
}

bool My_server::create_socket() {
	int opt = 1;
	_s_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (_s_socket < 0)
		throw std::string("socket failed");
	if (fcntl(_s_socket, F_SETFL, O_NONBLOCK | FD_CLOEXEC) == -1)
		throw std::string("fcntl failed");
	if (setsockopt(_s_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    	throw std::string("setsockopt failed");
	if (bind(_s_socket, (struct sockaddr *)&_s_addr, sizeof(_s_addr)) == -1)
		throw std::string("bind failed");
	if (listen(_s_socket, 128) < 0)
		throw std::string("listen failed");
	_pollfds.push_back(create_pollfd(_s_socket));
	return true; 
}

pollfd	My_server::create_pollfd(int fd)
{
	pollfd p;
	p.events = POLLIN;
	p.fd = fd;
	p.revents = 0;
	return p;
}

void My_server::start()
{
	init();
	create_socket();
	accept_loop();
}

void My_server::setPort(const int new_port) { this->_port = new_port; }

int	My_server::request(int index)
{
	if (index == 0)
	{
		std::cout << "Client connected " << std::endl;
		int client_fd = accept(_s_socket, NULL, NULL);
		if (client_fd >= 0)
		{
			if (fcntl(client_fd, F_SETFL, O_NONBLOCK) == -1)
				throw std::string("fcntl failed");
			_pollfds.push_back(create_pollfd(client_fd));
			_client.push_back(client(client_fd));
		}
		return index + 1;
	}

	char buffer[1025];
	int n = read(_pollfds[index].fd, buffer, 1024);

	if (n <= 0)
	{
		if (n == -1 &&  (errno != EAGAIN && errno != EWOULDBLOCK))
				return index + 1;
		close(_pollfds[index].fd);
		_pollfds.erase(_pollfds.begin() + index);
		_client.erase(_client.begin() + index - 1);
		return index;
	}

	buffer[n] = '\0';

	std::cout << "[RAW] " << buffer << std::endl;
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
	_pollfds[index].events |= POLLOUT;
	return index + 1;
}

int My_server::respons(int index)
{
	int fd = _pollfds[index].fd;
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
		_pollfds.erase(_pollfds.begin() + index);
		_client.erase(_client.begin() + index - 1);
	}
	_pollfds[index].events &= ~POLLOUT;
	return index;
}

bool    My_server::accept_loop()
{
	std::cout << "\nServer start ...\n";
	while (g_running)
	{
		int n = poll(_pollfds.data(), _pollfds.size(), _timeout_ms);

		if (n <= 0)	continue;

		size_t i = 0;
		while (i < _pollfds.size())
		{
			if (_pollfds[i].revents & POLLIN)
			{
				i = request(i);
			}
			else if (_pollfds[i].revents & POLLOUT)
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

