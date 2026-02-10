#include "../hpp/Client.hpp"

Client::~Client() { }

Client::Client() :
	timeOut(std::time(NULL)),
	best_location_index(-1),
	server_conf_key(-1),
	fd(-1),
	statuc_code(-1),
	is_dir(false),
	is_cgi(false),
	end_request(false)
{ }

Client::Client(const Client & obj)
{
	this->best_location_index = obj.best_location_index;
	this->fd = obj.fd;
	this->end_request = obj.end_request;
	this->buffer = obj.buffer;
	this->outbuf = obj.outbuf;
	this->server_conf_key = obj.server_conf_key;
	this->statuc_code = obj.statuc_code;
	this->timeOut = obj.timeOut;
	this->request = obj.request;
	this->is_dir = obj.is_dir;
}

Client::Client(int FD) :
	best_location_index(-1),
	server_conf_key(-1),
	fd(FD),
	statuc_code(0),
	is_dir(false),
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
		this->best_location_index = obj.best_location_index;
		this->server_conf_key = obj.server_conf_key;
		this->fd = obj.fd;
		this->statuc_code = obj.statuc_code;
		this->end_request = obj.end_request;
		this->buffer = obj.buffer;
		this->outbuf = obj.outbuf;
		this->timeOut = obj.timeOut;
		this->request = obj.request;
		this->is_dir = obj.is_dir;
	}
	return *this;
}
