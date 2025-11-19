#include "PollFds.hpp"

PollFds::~PollFds() {}

PollFds::PollFds(int fd) : val(false)
{
    this->pollfd.fd = fd;
    this->pollfd.revents = 0;
}

int	PollFds::getFd() const {
    return this->pollfd.fd;
}

std::string & PollFds::getBuffer() {
    return (this->buffer);
}

std::string & PollFds::getOutbuf() {
    return (this->outbuf);
}

void    PollFds::setBuffer(const std::string & str)
{
    this->buffer.append(str);
}
void    PollFds::setOutbuf(const std::string & str)
{
    this->outbuf.append(str);
}

bool    & PollFds::getBool() {
    return this->val;
}