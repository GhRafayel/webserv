#include "../hpp/Client.hpp"

Client::~Client() { }

Client::Client() : cgi_run(false), cgi_pid(0),
	best_location_index(-1),
	server_conf_key(-1),
	fd(-1),
	statuc_code(-1),
	is_dir(false),
	is_cgi(false),
	end_request(false)
{
	timeOut = std::time(NULL);
	in_pipe[0] = -1;
	in_pipe[1] = -1;
	out_pipe[0] = -1;
	out_pipe[1] = -1;
}

Client::Client(const Client & obj)
{
	this->cgi_run =				obj.cgi_run;
	this->cgi_pid = 			obj.cgi_pid;
	this->out_pipe[0]  =		obj.out_pipe[0];
	this->out_pipe[1]  =		obj.out_pipe[1];
	this->in_pipe[0]  =		obj.in_pipe[0];
	this->in_pipe[1]  =		obj.in_pipe[1];
	//////////////////////////////////////////
	this->best_location_index = obj.best_location_index;
	this->fd = 					obj.fd;
	this->end_request = 		obj.end_request;
	this->buffer = 				obj.buffer;
	this->outbuf = 				obj.outbuf;
	this->cgibuf = 				obj.cgibuf;
	this->server_conf_key = 	obj.server_conf_key;
	this->statuc_code = 		obj.statuc_code;
	this->timeOut = 			obj.timeOut;
	this->request = 			obj.request;
	this->is_dir = 				obj.is_dir;
	this->is_cgi = 				obj.is_cgi;
	this->cgi_type = 			obj.cgi_type;
	this->query = 				obj.query;
}

Client::Client(int FD) : cgi_run(false), cgi_pid(0),
	best_location_index(-1),
	server_conf_key(-1),
	fd(FD),
	statuc_code(0),
	is_dir(false),
	is_cgi(false),
	end_request(false), 
	buffer(""),
	outbuf("") 
{ 
	in_pipe[0] = -1;
	in_pipe[1] = -1;
	out_pipe[0] = -1;
	out_pipe[1] = -1;
	timeOut = std::time(NULL);
}

Client & Client::operator = (const Client & obj)
{
	if (this != & obj)
	{
		this->cgi_run =				obj.cgi_run;
		this->cgi_pid = 			obj.cgi_pid;
		this->out_pipe[0]  =		obj.out_pipe[0];
		this->out_pipe[1]  =		obj.out_pipe[1];
		this->in_pipe[0]  =			obj.in_pipe[0];
		this->in_pipe[1]  =			obj.in_pipe[1];
		//////////////////////////////////////////
		this->best_location_index = obj.best_location_index;
		this->server_conf_key = 	obj.server_conf_key;
		this->fd = 					obj.fd;
		this->statuc_code = 		obj.statuc_code;
		this->end_request = 		obj.end_request;
		this->buffer = 				obj.buffer;
		this->cgibuf = 				obj.cgibuf;
		this->outbuf = 				obj.outbuf;
		this->timeOut = 			obj.timeOut;
		this->request = 			obj.request;
		this->is_dir = 				obj.is_dir;
		this->is_cgi = 				obj.is_cgi;
		this->cgi_type =			obj.cgi_type;
		this->query = 				obj.query;

	}
	return *this;
}
