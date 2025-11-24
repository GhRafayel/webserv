#include "Client.hpp"


client::~client() { }

client::client() : fd(-1), val(false), buffer(""), outbuf("") {}

client::client(int FD) : fd(FD), val(false), buffer(""), outbuf("") {}