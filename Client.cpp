#include "Client.hpp"

Client::~Client() { }

Client::Client() :
	server_conf_key(-1),
	fd(-1),
	statuc_code(-1),
	is_dir(false),
	end_request(false)
{ timeOut = std::time(NULL);}

Client::Client(const Client & obj)
{
	this->fd = obj.fd;
	this->end_request = obj.end_request;
	this->buffer = obj.buffer;
	this->outbuf = obj.outbuf;
	this->server_conf_key = obj.server_conf_key;
	this->statuc_code = obj.statuc_code;
	this->timeOut = obj.timeOut;
}

Client::Client(int FD) :
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
		this->server_conf_key = obj.server_conf_key;
		this->fd = obj.fd;
		this->statuc_code = obj.statuc_code;
		this->end_request = obj.end_request;
		this->buffer = obj.buffer;
		this->outbuf = obj.outbuf;
		this->timeOut = obj.timeOut;
	}
	return *this;
}
