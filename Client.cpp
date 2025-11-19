#include "Client.hpp"


client::~client() { }

client::client() : fd(-1), val(false) {}

client::client(int FD) : fd(FD), val(false) {}