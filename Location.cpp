#include "Location.hpp"

Location::~Location() {  this->_config.clear(); }

Location::Location() :
_config(), _GET(false), _POST(false), _DELETE(false), _autoIndex(false), 
_error_massage("Configuration Error:\nAn invalid line was detected in the location block of the configuration file: > ")
 {}	

Location::Location(std::vector<std::string> & array) :
_config(array), _GET(false), _POST(false), _DELETE(false), _autoIndex(false),
_error_massage("Configuration Error:\nAn invalid line was detected in the location block of the configuration file: > ")
{
	init();
}	

Location::Location(const Location & obj) : StringUtils() ,
	_error_massage("Configuration Error:\nAn invalid line was detected in the location block of the configuration file: > ")
{
	this->_GET = obj._GET;
	this->_POST = obj._POST;
	this->_DELETE = obj._DELETE;
	this->_autoIndex = obj._autoIndex;
	this->_root = obj._root;
	this->_redirection = obj._redirection;
	this->_index = obj._index;
	this->_location = obj._location;
}

Location & Location::operator = (const Location & obj)
{
	if (this != & obj)
	{
		this->_GET = obj._GET;
		this->_POST = obj._POST;
		this->_DELETE = obj._DELETE;
		this->_autoIndex = obj._autoIndex;
		this->_root = obj._root;
		this->_redirection = obj._redirection;
		this->_index = obj._index;
		this->_location = obj._location;
	}
	return *this;
}

bool	Location::is_line_valid(std::string & line)
{
	if (line[line.size() - 1] == ';')
	{
		line = line.substr(0, line.size() - 1);
		return true;
	}
	return false;
}

void	Location::init()
{
	std::string key;
	std::string value;
	for (size_t i = 0; i < _config.size(); i++)
	{
		size_t id = _config[i].find(" ");
		if (id != std::string::npos)
		{
			key = trim(_config[i].substr(0, id), " ");
			value = trim(_config[i].substr(id, _config[i].size()), " ");
			if (key == "location" || is_line_valid(value))
				callFunctionByName(key, value);
			else
				throw std::runtime_error(_error_massage += _config[i]);
		}
	}
}

void    Location::location(std::string & str)
{
	if (str.empty())
		throw std::runtime_error(_error_massage + str);
	this->_location = str;
}

void	Location::redirect(std::string & str)
{
	this->_redirection = str;
}

void    Location::root(std::string & str) {
	this->_root = str;
}

void    Location::index(std::string & str) {
	this->_index = str;
}

void    Location::autoindex(std::string & str) {

	if (str == "on")
		this->_autoIndex = true;
	else if (str == "off")
		this->_autoIndex = false;
	else if (str.empty())
		return;
	else
		throw std::runtime_error(_error_massage + str);
}

void    Location::methods(std::string & str)
{
	if (str.empty()) return;

	std::vector<std::string> temp = split(str, " ", true);
	for (size_t i = 0; i < temp.size(); i++)
	{
		if (temp[i] == "GET")
		{
			this->_GET = true;
		}
		else if (temp[i] == "POST")
		{
			this->_POST = true;
		}
		else if (temp[i] == "DELETE")
		{
			this->_DELETE = true;
		}
		else
			throw std::runtime_error(_error_massage + str);
	}
}

void Location::callFunctionByName(const std::string & fun_name, std::string & arg)
{
	std::map<std::string, void (Location::*)(std::string &)> func_map;

	func_map.insert(std::make_pair("location", &Location::location));
	func_map.insert(std::make_pair("root", &Location::root));
	func_map.insert(std::make_pair("index", &Location::index));
	func_map.insert(std::make_pair("autoindex", &Location::autoindex));
	func_map.insert(std::make_pair("methods", &Location::methods));
	func_map.insert(std::make_pair("redirect", &Location::redirect));

	try
	{
		std::map<std::string, void (Location::*) (std::string &)>::iterator	it = func_map.find(fun_name);
		if (it == func_map.end())
			throw std::runtime_error(_error_massage + fun_name + " " + arg);
		if (arg.empty())
			return ;
		(this->*(it->second))(arg);
	}
	catch(const std::exception & e)
	{
		std::cerr << e.what() << '\n';
	}
}
