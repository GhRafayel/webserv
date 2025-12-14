#include "Server.hpp"

Server::~Server() {}

Server::Server() : 
    _locations(),
    _root("www/public"),
    _server_name("localhost"),
    _body_max_size(10000),
    _error_404("/www/errors/404.html"),
    _error_500("/www/errors/500.html"),
    _port(8080)
{
    _addr.sin_family = AF_INET;
    _addr.sin_addr.s_addr = INADDR_ANY;
    _addr.sin_port = htons(8080);
}

Server::Server(const Server & obj) : 
    _methods(obj._methods),
    _locations(obj._locations),
    _root(obj._root),
    _index(obj._index),
    _server_name(obj._server_name),
    _body_max_size(obj._body_max_size),
    _error_404(obj._error_404),
    _error_500(obj._error_500),
    _port(obj._port)
{
    _addr.sin_family = obj._addr.sin_family;
    _addr.sin_addr.s_addr = obj._addr.sin_addr.s_addr;
    _addr.sin_port = obj._addr.sin_port;
}

Server & Server::operator = (const Server & obj)
{
    if (this != &obj)
    {
        this->_locations = obj._locations;
        this->_root = obj._root;
        this->_server_name = obj._server_name;
        this->_body_max_size = obj._body_max_size;
        this->_error_404 = obj._error_404;
        this->_error_500 = obj._error_500;
        this->_port = obj._port;
        this->_addr.sin_family = obj._addr.sin_family;
        this->_addr.sin_addr = obj._addr.sin_addr;
        this->_addr.sin_port = obj._addr.sin_port;
        this->_methods = obj._methods;
    }
    return *this;
}

bool	Server::get_method(const std::string & method)
{
	std::map<std::string,bool>::iterator it = _methods.find(method);
	if (it == _methods.end())
		return false;
	return it->second;
}