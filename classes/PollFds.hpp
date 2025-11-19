#pragma once

#include "Client.hpp"
#include <iostream>
#include <poll.h>

class PollFds 
{
	private:
		bool		val;
		std::string	buffer;
		std::string	outbuf;
		pollfd		pollfd;
	public:
		int				getFd() const;
		
		void			setBuffer(const std::string &);
		void			setOutbuf(const std::string &);
		std::string		&getBuffer();
		std::string		&getOutbuf();
		bool			&getBool();

		~PollFds();
		PollFds(int fd);
};