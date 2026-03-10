*This project has been created as part of the 42 curriculum by rghazary, dvavryn.*

# webserv

## Description

**webserv** is a fully functional HTTP/1.0 web server written in C++98, built from scratch as part of the 42 school curriculum. The goal of the project is to gain a deep understanding of how web servers work by implementing one without relying on any externam networking libraries.

The server handles concurrent client connections using a non-blicking I/O model with `poll()`, parses HTTP requests, serves static files, handles file uploads, executes CGI scripts, and manages multiple virtual server configurations — all driven by an Nginx-inspired configuration file.

Key characteristics:
- Non-blocking, event-driven architecture using `poll()`
- Support for multiple virtual servers on different ports
- HTTP methods: `GET`, `POST`, `DELETE`
- CGI execution (PHP, Python3)
- Static file serving with directory listing
- Configurable error pages, body size limits and location blocks
- Graceful shutdown on `SIGINT`/`SIGTERM`

## Instructions

### Requirements

- A C++98-compatible compiler (e.g. `c++`, `g++` or `clang++`)
- `make`
- `php-cgi` (for CGI support)
- `python3` (for CGI support)

### Compilation

```bash
make
```

This produces the `webserv` binary in the project root.

To clean build artifacts:

```bash
make clean		# removes object files
make fclean		# removes object files and binary
make re			# full rebuilt
```

### Execution

```bash
./webserv [confi_file]
```

- If no config file is provided, the server looks for `conf/server.conf` by default.
- The config file uses an Nginx-like syntax. See [conf/default.conf](conf/default.conf) for an annotated example.

**Minimal configuration example:**

```nginx
http
{

	server {
	
		listen  8080;

		client_max_body_size  1048576;

		allow_method GET POST;

		server_name  localhost8080;
		
		root www/;
		index index.html;

		cgi_path /opt/homebrew/bin/ /usr/bin/;
		
		error_page_404  www/errors/404.html;
		error_page_500  www/errors/500.html;

		location /img {
			root  www/img/;
		}

		location /upload {
			root www/DB/;
			allow_methods  POST DELETE;
		}
		
		location /cgi-bin {

			root www/cgi-bin/;

			cgi_extension .php;
			cgi_extension .py;

			allow_methods  POST GET;
		 }
	}
}
```

Once running, open your browser at `http://localhost:8080`.

## Features

- **HTTP/1.0** request parsing and response generation
- **GET** — serve static files and directory listings
- **POST** — handle form submissions and file uploads
- **DELETE** — delete files on the server
- **CGI** — execute `.php` and `.py` scripts
- **Multiple servers** — host several virtual servers simultaneously, each with its own port, root and settings
- **Location blocks** — per-rout configuration (methods, redirects, root override)
- **Custom error pages** — configurable 404 and 500 error pages
- **Connection timeout** — automatic cleanup of stale connections
- **Non-blocking I/O** — `poll()`-based event loop; no thread-per-connection

## Usage Examples

**Serve static content:**
```
GET http://localhost:8080/index.html
```

**Access the TODO app:**
```
GET http://localhost:8080/Todo/
```

**Run a PHP CGI script:**
```
GET http://localhost:8080/cgi-bin/info.php
```

**Upload a file (POST):**
```bash
curl -X POST http://localhost:8080/upload -F "file=@myfile.txt"
```

**Delete a file:**
```bash
curl -X DELETE http://localhost:8080/path/to/file.txt
```

## Resources

### Documentation & References

- [RFC 1945 — HTTP/1.0](https://www.rfc-editor.org/info/rfc1945) — the specification this servers implements
- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/) — comprehensive guide to BSD sockets in C
- [The Linux `poll()` man page](https://man7.org/linux/man-pages/man2/poll.2.html)
- [RFC 3875 — CGI specification](https://www.rfc-editor.org/info/rfc3875)
- [Nginx configuration concepts](https://nginx.org/en/docs/beginners_guide.html) — used as inspiration for the config file syntax
- [MDN HTTP documentation](https://developer.mozilla.org/en-US/docs/Web/HTTP) — HTTP header, methods and status codes reference

### AI Usage

- **Debugging** — analysing crash/hand scenarios in the event loop and CGI subprocess
- **Understanding specifications** — clarifying ambiguous parts of the RFCS

AI was not used to generate core implementation code. All networing, parseing and HTTP logic was written by the project authors.