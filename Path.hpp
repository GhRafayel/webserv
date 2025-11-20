#ifndef PATH_HPP
#define PATH_HPP

#include <iostream>
#include <stdlib.h>

class Path
{
    private:
        Path(const Path&);
        Path & operator = (const Path&);
    public:
        ~Path();
        Path();
        char    *absolutePath(const std::string &);
        bool    is_dir();
        bool    is_file();

};

#endif