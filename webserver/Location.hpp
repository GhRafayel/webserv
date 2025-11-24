#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <iostream>
#include <vector>
#include <map>

class Location
{
    private:
        Location();
        void       init(std::vector<std::string> &);
    public:
        std::map<std::string, std::string>  location;
        std::map<std::string, bool>         methods;

        ~Location();
       
        Location(std::vector<std::string> &);
        Location(const Location &); 
        Location & operator = (const Location &);
};

#endif
