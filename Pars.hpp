#pragma once

#include <iostream>
#include <sstream>
#include <vector>

class Pars : public std::string
{
    public:
    static std::vector<std::string> split(std::string &, const std::string &);
    static std::vector<std::string> trimArray(std::vector<std::string> &);
    static std::string              trim(std::string &);
    static std::string              chang_char(std::string &, const char, const char);
    static bool                     recieve(const std::string &);
    static bool                     is_digitS(const std::string &);
    static int                      str_to_int(std::string &);
};

