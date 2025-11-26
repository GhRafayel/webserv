#include "Client.hpp"

Client::~Client() { }

Client::Client(const Client & obj)
{
    this->fd = obj.fd;
    this->end_request = obj.end_request;
    this->buffer = obj.buffer;
    this->outbuf = obj.outbuf;
    this->server_conf_key = obj.server_conf_key;
    this->statuc_code = obj.statuc_code;
}

Client::Client(int FD) :
    server_conf_key(-1),
    fd(FD),
    statuc_code(0),
    end_request(false), 
    buffer(""), 
    outbuf("") {}