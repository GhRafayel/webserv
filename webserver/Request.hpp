#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "StringUtils.hpp"
#include <sys/socket.h>
#include "Client.hpp"
#include "Server.hpp"
#include <iostream>
#include <map>


class Request : public StringUtils
{
	private:
		Client								&client_ref;
		std::string							method;
		std::string							path;
		std::string							protocol;
		std::map<std::string, std::string>	request;

		Request(const Request &);
		Request &	operator = (const Request &);
	public:
		

		void								foo(Server * );
		void								analize_request();
		std::string							getMethod();
		std::string							getPath();
		std::string 						getProtocol();
		std::map<std::string, std::string>	&getRequest();

		~Request();
		Request(Client &);
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
	{ "css",  "text/css" },
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