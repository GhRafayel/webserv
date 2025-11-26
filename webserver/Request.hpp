#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "StringUtils.hpp"
#include <sys/socket.h>
#include "Client.hpp"
#include <iostream>
#include <map>


class Request : public StringUtils
{
	private:
		std::string str_request;
		Request(const Request &);
		Request &	operator = (const Request &);

		bool		end_of_request();
		int			to_read(int);
	public:
		~Request();
		Request(std::map<int, Client>::iterator it);
		Request();
		// void	analize_request();
};

#endif


/*
typedef struct {
	const char *ext;
	const char *mime;
} mime_entry;

static const mime_entry mime_map[] = {
	{ "html", "text/html" },
	{ "htm",  "text/html" },
	{ "css",  "text/css" },        // ոչ text.css
	{ "js",   "application/javascript" },
	{ "json", "application/json" },
	{ "jpg",  "image/jpeg" },
	{ "jpeg", "image/jpeg" },
	{ "png",  "image/png" },
	{ "gif",  "image/gif" },
	{ "svg",  "image/svg+xml"},
	{ "txt",  "text/plain" },
	{ "pdf",  "application/pdf"},
	{ "webp", "image/webp" },
	{ "woff", "font/woff" },
	{ "woff2","font/woff2" },
	{ "xml",  "application/xml" },
	{ "zip",  "application/zip" },
	{ NULL,   NULL }               // ավարտ
};

*/