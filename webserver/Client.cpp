#include "Client.hpp"


Client::~Client() { }

Client::Client() : fd(-1), val(false), buffer(""), outbuf("") {}

Client::Client(int FD) : fd(FD), val(false), buffer(""), outbuf("") {}