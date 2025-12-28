1. Տեսական պատրաստություն (օր 1-2)

Կարդա RFC 1945 (HTTP/1.0) – բավական է առաջին 4 բաժինը

Իմացի՛ր NGINX-ով «թելնետ» փորձարկումը

telnet localhost 80

GET / HTTP/1.0⏎

Host: localhost⏎
⏎
Նայի՛ր թե NGINX-ն ինչ headers է ուղարխում, ինչ status line, 

ինչպես է անում 404, 403, 413, 500 էջերը

--------------------------------------------------------

2. Նվազագույն սկելետ (օր 3-4)

    argv[1]-ով վերցնել config ֆայլի path, եթե չկա – ./conf/default.conf

    socket() → bind() → listen() մեկ պորտի վրա (օր. 8080)

    Պահել vector<Server>

Գլխավոր loop-ը poll() (կամ select()/epoll())

    vector<struct pollfd>

    pfds[0].fd = listen_sock; pfds[0].events = POLLIN;

    poll(pfds.data(), pfds.size(), -1)

    Եթե revents & POLLIN և fd == listen_sock → accept() → նոր client-ի fd-ն նույնպես դնել pfds-ում O_NONBLOCK

Client-ի կողմից POLLIN եկել է

    recv() մինչև \r\n\r\n ստանալը

    parse-ել առաջին տողը «GET / HTTP/1.0»

    ուղարկել հերթապահ 200 OK + «Hello world»

    close() և հեռացնել pfds-ից  

Ստուգիր
    բրաուզերով http://localhost:8080/ → պետք է ցույց տա Hello world

    telnet-ով նույնը

Կոնֆիգուրացիայի պարսեր (օր 5)

NGINX-անալոգ ֆայլ.

    server {
        listen 127.0.0.1:8080;
        server_name localhost;
        error_page 404 /errors/404.html;
        client_max_body_size 1048576;
        location / {
            root   ./www;
            index  index.html;
            methods GET POST DELETE;
            autoindex off;
        }
        location /upload {
            root   ./uploads;
            methods POST;
            upload_store ./uploads;
        }
        location ~ \.php$ {
            methods GET POST;
            cgi_pass /usr/bin/php-cgi;
        }
    }
    Գրիր պարզ lexer + map<string, vector<Server>
    Դուրս բեր. port, host, root, index, error_page, 
    client_max_body_size, allowed_methods, 
    autoindex, upload_store, cgi_pass

4. Պատասխանի կառուցում (օր 6-7) Request-ից հետո

    ստուգել method-ը allowed_methods-ում

    կառուցել ֆիզիկական ճանապարհ = location.root + URI

    եթե դիրեկտորիա է և index ֆայլ կա → կարդալ index

    եթե դիրեկտորիա է և autoindex on → սարքել HTML ցուցակ

    եթե ֆայլ չկա → ուղարկել error_page 404 (եթե կա disk-ում, կարդալ, եթե ոչ – built-in HTML)

Status line + headers
    HTTP/1.1 200 OK\r\n
    Content-Length: 1234\r\n
    Content-Type: text/html\r\n
    Connection: close\r\n
    \r\n

Հետո body

Բոլոր read/write սոքետների վրա՝ միայն poll()-ով, NONBLOCK, errno-ով չես ստուգում (օրենք)

    Եթե send() ամբողջը չի գնացել, մնացորդը պահես client-ի struct-ում և POLLOUT ավելացրու

5. POST + file upload (օր 8)

    Content-Type: multipart/form-data; boundary=----WebKitFormBoundary...

    Փնտրել boundary, կտրել մասերը, գրել disk → upload_store/unique_name

    Content-Length > client_max_body_size → 413 Payload Too Large

6. DELETE մեթոդ (օր 8)

    std::remove(path.c_str())

    հաջող → 204 No Content, չհաջող → 403 / 404

7. CGI (օր 9-10)


    fork() միայն այստեղ է թույլատրված

    pipe() երկու զույգ (parent↔child)

    env փոխանցել REQUEST_METHOD, PATH_INFO, QUERY_STRING, CONTENT_LENGTH, CONTENT_TYPE, SERVER_PROTOCOL...

    POST-ի body-ն լրիվ կարդալ, ապա write() դեպի pipe (child-ի stdin)

    Child-ը dup2() → execve(cgi_path, argv, envp)

    Parent-ը poll-ով կարդում CGI-ի stdout

    Չկա Content-Length → կարդալ մինչև EOF

    Status line առաջին տողում կարող է լինել «Status: 404 Not Found» → parse ու սարքել հիմնական response

---------------------------------------------------------------------------------------------------------------------

8. Նոն-բլոկինգ rules (ամբողջ պրոյեկտում)


    բոլոր սոքետ fd-ներ fcntl(fd, F_SETFL, O_NONBLOCK)

    poll() loop-ում ամեն fd-ի համար հետևել POLLIN | POLLOUT

    երբեք չկանչել read/recv/write/send առանց poll()-ի թույլտվության

    errno ստուգել արգելված է → եթե recv() < 0 և errno == EAGAIN → պարզապես հետ կանգնիր, poll()-ը կվերադառնա նորից

    disk ֆայլերի read()/write()-ը կարող ես անել ուղիղ (blocking-ով), որովհետեւ subject-ում ասված է ազատված են

---------------------------------------------------------------------------------------------------------------------

9. Բազմապորտ, բազմասերվեր (օր 11)

    config-ում մի քանի listen բլոկ

    vector<Server>

    pfds-ում լսող սոքետները լինելու են առանձին fd, բայց նույն poll() զանգի մեջ

    accept()-ից հետո client-ի struct-ում նշել որ server-ին է պատկանում (ip:port)
---------------------------------------------------------------------------------------------------------------------

10. Էռոր փեյջեր, resilience, stress test (օր 12-13)

    400, 403, 404, 413, 500, 502, 505 → built-in HTML + կոնֆիգով տրված ֆայլ

    Python-ով սկրիպտ.
        1000 կոնկուրենտ կապ (threading/requests)

        ատ մեծ ֆայլեր upload

        կիսատ request, անսպասելի disconnect
    
    Սերվերը չպետք է կախվի, չպետք է արյուն leaking լինի

---------------------------------------------------------------------------------------------------------------------

11. Թեստային ֆայլերի փաթեթ (օր 14)

    conf/
        default.conf
    www/
        index.html
        50x.html
    errors/
        404.html
    uploads/   (chmod 777)
    cgi/
        test.php  (#!/usr/bin/php-cgi)
    
    Բրաուզերով.

        GET / → index.html

        GET /autoindex/ → ցուցակ

        POST /upload (form) → ֆայլը հայտնվում uploads/-ում

        DELETE /upload/file.txt → ֆայլը ջնջվում է

        GET /test.php → Hello from PHP
    
    telnet-ով ստուգել 404, 413, Wrong method

---------------------------------------------------------------------------------------------------------------------

12. Վերջնական ստուգում

    make → ./webserv conf/default.conf

    բրաուզեր, telnet, Python stress, NGINX համեմատություն (headers, status line)

    valgrind --leak-check=full → 0 leaks, 0 errors

    clang -Wall -Wextra -Werror -std=c++98



===================================================================

// OLD (օգտագործում է errno)
if (n == -1 && (errno != EAGAIN && errno != EWOULDBLOCK))

// NEW (առանց errno)
if (n == -1 && !fd_is_non_blocking(fd))

bool fd_is_non_blocking(int fd)
{
    int fl = fcntl(fd, F_GETFL);
    return (fl & O_NONBLOCK);
}


- if (n == -1 && (errno != EAGAIN && errno != EWOULDBLOCK))
+ if (n == -1 && !fd_is_non_blocking(fd))


read 1 → n=1024   (մեջը չկա \r\n\r\n)
read 2 → n=776    (մնացած + \r\n\r\n)

struct client {
    std::string req_buf;   // կուտակում ենք մինչև \r\n\r\n
    std::string outbuf;

};

std::string tmp(buf, n);
c.req_buf += tmp;
if (c.req_buf.find("\r\n\r\n") != std::string::npos)
    // հարցումը լրիվ է, հիմա կարող եք parse անել





//////////////////// GET //////////////////////////

GET – only the codes you really have to care about:
200 OK
206 Partial Content (if you support Range)
400 Bad Request
401 Unauthorized
403 Forbidden
404 Not Found
411 Length Required (only if you demand Content-Length on GET with body)
413 URI Too Long
414 URI Too Long
500 Internal Server Error
503 Service Unavailable

//////////////////// POST //////////////////////////

200 OK
201 Created
204 No Content
400 / 401 / 403 / 404
413 Payload Too Large
500 / 503

//////////////////// DELETE //////////////////////////

200 OK
204 No Content
202 Accepted
400 / 401 / 403 / 404
500 / 503