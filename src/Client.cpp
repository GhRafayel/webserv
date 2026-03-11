#include "../hpp/Client.hpp"

Client::~Client() { }

Client::Client() : read_size(0),
	best_location_index(-1),
	server_conf_key(-1),
	fd(-1),
	cgi_fd(-1),
	statuc_code(-1),
	is_dir(false),
	is_cgi(false),
	cgi_runer(false),
	end_request(false)
{ timeOut = std::time(NULL);}

Client::Client(const Client & obj)
{
	this->read_size =			obj.read_size;
	this->pid =					obj.pid;
	this->timeOut = 			obj.timeOut;
	this->best_location_index = obj.best_location_index;
	this->server_conf_key = 	obj.server_conf_key;
	this->fd = 					obj.fd;
	this->cgi_fd = 				obj.cgi_fd;
	this->statuc_code = 		obj.statuc_code;
	this->is_dir = 				obj.is_dir;
	this->is_cgi = 				obj.is_cgi;
	this->cgi_runer	=			obj.cgi_runer;
	this->end_request = 		obj.end_request;
	this->query = 				obj.query;
	this->buffer = 				obj.buffer;
	this->outbuf = 				obj.outbuf;
	this->cgibuf = 				obj.cgibuf;
	this->cgi_type = 			obj.cgi_type;
	this->best_mach =			obj.best_mach;
	this->method =				obj.method;
	this->request = 			obj.request;
}

Client::Client(int FD) : read_size(0),
	best_location_index(-1),
	server_conf_key(-1),
	fd(FD),
	cgi_fd(-1),
	statuc_code(0),
	is_dir(false),
	is_cgi(false),
	cgi_runer(false),
	end_request(false), 
	buffer(""),
	outbuf("") 
{ 
	timeOut = std::time(NULL);
}

Client & Client::operator = (const Client & obj)
{
	if (this != & obj)
	{
		this->read_size =			obj.read_size;
		this->pid =					obj.pid;
		this->timeOut = 			obj.timeOut;
		this->best_location_index = obj.best_location_index;
		this->server_conf_key = 	obj.server_conf_key;
		this->fd = 					obj.fd;
		this->cgi_fd = 				obj.cgi_fd;
		this->statuc_code = 		obj.statuc_code;
		this->is_dir = 				obj.is_dir;
		this->is_cgi = 				obj.is_cgi;
		this->cgi_runer =			obj.cgi_runer;
		this->end_request = 		obj.end_request;
		this->query = 				obj.query;
		this->buffer = 				obj.buffer;
		this->outbuf = 				obj.outbuf;
		this->cgibuf = 				obj.cgibuf;
		this->cgi_type = 			obj.cgi_type;
		this->best_mach =			obj.best_mach;
		this->method =				obj.method;
		this->request = 			obj.request;
	}
	return *this;
}
