#pragma onec

#include <iostream>
#include "Pars.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>

class Request : public Pars
{
    private:
        
    public:
        ~Request();
        int         connect(int);
        void        nonBlock(int);
        std::string to_read(int);
};