#ifndef REQUEST_HPP
#define REQUEST_HPP

class Request
{
    private:
        Request(const Request &);
        Request & operator = (const Request &);
    public:
        ~Request();
        Request();
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