#include "Location.hpp"

Location::~Location() {
	args.clear();
	allowed_methods.clear();
}

Location::Location(std::vector<std::string> & arr, int i)
	: count(0), valid(true), args(), allowed_methods()
{
	args.insert(std::make_pair("location", ""));
	args.insert(std::make_pair("root", ""));
	args.insert(std::make_pair("index", ""));
	args.insert(std::make_pair("redirect", ""));

	allowed_methods.insert(std::make_pair("autoindex", false));
	allowed_methods.insert(std::make_pair("GET", false));
	allowed_methods.insert(std::make_pair("POST", false));
	allowed_methods.insert(std::make_pair("DELETE", false));
	init(arr, i);
}

Location::Location(const Location & obj)
	 : StringUtils(), count(obj.count), valid(obj.valid), args(obj.args), allowed_methods(obj.allowed_methods) {
}

Location & Location::operator = (const Location & obj)
{
	if (this != & obj)
	{
		std::map<std::string, std::string> temp_location(obj.args);
		this->args = temp_location;
		std::map<std::string, bool> temp_methods(obj.allowed_methods);
		this->allowed_methods = temp_methods;
		this->valid = obj.valid;
		this->count = obj.count;
	}
	return *this;
}

void    Location::location(std::string & str) {
	std::map<std::string, std::string>::iterator it = args.find("location");
	it->second = str;
}

void	Location::redirect(std::string & str)
{
	std::map<std::string, std::string>::iterator it = args.find("redirect");
	it->second = str;
}

void    Location::root(std::string & str) {
	std::map<std::string, std::string>::iterator it = args.find("root");
	it->second = str;
}

void    Location::index(std::string & str) {
	std::map<std::string, std::string>::iterator it = args.find("index");
	it->second = str;
}

void    Location::autoindex(std::string & str) {

	std::map<std::string, bool>::iterator it = allowed_methods.find("autoindex");
	if (str == "on")
		it->second = true;
	else if (str == "off")
		it->second = false;
	else
		throw std::runtime_error("autoindex is invalid");
}

void    Location::methods(std::string & str) {
	std::vector<std::string> temp = split(str, " ", true);
	for (size_t i = 0; i < temp.size(); i++)
	{
		if (temp[i] == "GET")
		{
			std::map<std::string, bool>::iterator it = allowed_methods.find("GET");
			it->second = true;
		}
		else if (temp[i] == "POST")
		{
			std::map<std::string, bool>::iterator it = allowed_methods.find("POST");
			it->second = true;
		}
		else if (temp[i] == "DELETE")
		{
			std::map<std::string, bool>::iterator it = allowed_methods.find("DELETE");
			it->second = true;
		}
	}
}

void	Location::control() {
	
	if (!args.find("location")->second.size())
		this->valid = false;
	if (!args.find("root")->second.size())
		this->valid = false;
	if (!args.find("index")->second.size())
		this->valid = false;
}

void    Location::init(std::vector<std::string> & arr, int in)
{
	for (size_t i = in; i < arr.size(); i++)
	{
		std::vector<std::string> temp = split(arr[i], "=", true);
		
		if (temp.size() == 2 && temp[0].size() && temp[1].size())
			call_member(temp[0], temp[1].c_str());
		temp.clear();
	}
	control();
}

void Location::call_member(const std::string & fun_name, const char * arg)
{        
	std::string fun_list[7] = 
	{ "location", "root",  "index", "autoindex", "methods", "redirect"};
	
	void (Location::*fun_ref[7]) (std::string &) =
	{
		&Location::location, &Location::root, &Location::index, 
		&Location::autoindex, &Location::methods, &Location::redirect
	};
	try
	{
		for (size_t i = 0; i < fun_list->size(); i++)
		{
			if (fun_list[i] == fun_name)
			{	
				std::string temp = arg;
				(this->*fun_ref[i])(temp);
				return;
			}
		}
	}
	catch(const std::exception & e)
	{
		std::cerr << e.what() << '\n';
		this->valid = false;
	}
}
