#pragma once

#include <iostream>

class Client
{
    public:
        int         fd;
        int         statuc_code;
        int         val;
        std::string buffer;
        std::string outbuf;
        ~Client();
        Client(int);
        Client();
};