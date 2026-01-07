# Webserv

## 📌 Description

**Webserv** is a project from **42 School** that consists of building a simple **HTTP web server** in **C++**, inspired by **Nginx** behavior.

The goal of this project is to understand how web servers work at a low level, including sockets, HTTP parsing, non-blocking I/O, and handling multiple clients simultaneously.

---

## ⚙️ Features

- HTTP/1.1 protocol support
- Non-blocking server using `poll()` (or equivalent)
- Multiple clients handled simultaneously
- Configurable server using a configuration file
- Support for:
  - GET, POST, DELETE methods
  - Static file serving
  - File upload
  - Custom error pages
  - Redirections
  - CGI execution
- Multiple virtual servers
- Proper request parsing and response building

---

## 🛠️ Technologies Used

- Language: **C++**
- Standard: **C++98**
- System calls:
  - `socket`
  - `bind`
  - `listen`
  - `accept`
  - `recv`
  - `send`
  - `poll`
  - `close`
- Operating system: **Linux / macOS**

---

## 📂 Project Structure

```text
.
├── config/
│   └── default.conf
├── src/
│   ├── main.cpp
│   ├── Server.cpp
│   ├── Client.cpp
│   ├── Request.cpp
│   ├── Response.cpp
│   └── CGI.cpp
├── hpp/
│   └── webserv.hpp
├── www/
│   └── index.html
├── Makefile
└── README.md
