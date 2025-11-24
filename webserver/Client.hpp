#pragma once

#include <iostream>

class client
{
    public:
        int         fd;
        int         statuc_code;
        bool        val;
        std::string buffer;
        std::string outbuf;
        ~client();
        client(int);
        client();
};