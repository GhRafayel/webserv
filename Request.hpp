#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include "Pars.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include "Client.hpp"

class Request : public Pars
{
    private:
        Request(const Request &);
        Request & operator = (const Request &);
    public:
        ~Request();
        Request();
        int			connect(int);
        void		nonBlock(int);
        void		analize_request(client &);
        std::string	to_read(int);
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
    { "svg",  "image/svg+xml" },
    { "txt",  "text/plain" },
    { "pdf",  "application/pdf" },
    { "webp", "image/webp" },
    { "woff", "font/woff" },
    { "woff2","font/woff2" },
    { "xml",  "application/xml" },
    { "zip",  "application/zip" },
    { NULL,   NULL }               // ավարտ
};

*/