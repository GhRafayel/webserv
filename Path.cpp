#include "Path.hpp"
Path::~Path() {}

Path::Path() {}

char *Path::absolutePath(const std::string & file_name)
{
    std::string path = "conf";
    char actualpath [1024];
    char *ptr = realpath((path + file_name).c_str(), actualpath);
    if (!ptr)
        return NULL;
    return ptr;
}

bool    Path::is_dir() {return true;}
bool    Path::is_file() {return true;}