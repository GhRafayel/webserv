#pragma once

#include <iostream>
#include <vector>

class Pars : public std::string
{
    public:
    static std::vector<std::string> split(std::string &, const std::string &);
    static std::string              trim(std::string &);
    static std::string              chang_char(std::string &, const char, const char);
    static bool                     end_req(const std::string & buffer);
};

