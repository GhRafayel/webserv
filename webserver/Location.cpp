#include "Location.hpp"

Location::~Location() {
    location.clear();
    methods.clear();
}

Location::Location(std::vector<std::string> & arr) : location(), methods() { init(arr); }

Location::Location(const Location & obj) : location(obj.location), methods(obj.methods) {}

Location & Location::operator = (const Location & obj)
{
    if (this != & obj)
    {
        std::map<std::string, std::string> temp_location(obj.location);
        this->location = temp_location;
        std::map<std::string, bool> temp_methods(obj.methods);
        this->methods = temp_methods;
    }
    return *this;
}

void    Location::init(std::vector<std::string> & arr)
{
    for (size_t i = 0; i < arr.size(); i++)
    {
        std::cout << arr[i] << std::endl;
    }
}