#include "Location.hpp"

Location::~Location() {  this->_config.clear(); }

Location::Location() : func_map(),_config(), _methods(), _autoIndex(false),
_root("/www/public/"), _redirection(""), _index("index.html"), _location("/"),
_error_massage("Configuration Error:\nAn invalid line was detected in the location block of the configuration file: > ")
 {
	func_map.insert(std::make_pair("location", &Location::location));
	func_map.insert(std::make_pair("root", &Location::root));
	func_map.insert(std::make_pair("index", &Location::index));
	func_map.insert(std::make_pair("autoindex", &Location::autoindex));
	func_map.insert(std::make_pair("methods", &Location::methods));
	func_map.insert(std::make_pair("method", &Location::methods));
	func_map.insert(std::make_pair("redirect", &Location::redirect));
 }

Location::Location(std::vector<std::string> & array) : func_map(),
_config(array), _methods(), _autoIndex(false),
_error_massage("Configuration Error:\nAn invalid line was detected in the location block of the configuration file: > ")
{
	func_map.insert(std::make_pair("location", &Location::location));
	func_map.insert(std::make_pair("root", &Location::root));
	func_map.insert(std::make_pair("index", &Location::index));
	func_map.insert(std::make_pair("autoindex", &Location::autoindex));
	func_map.insert(std::make_pair("methods", &Location::methods));
	func_map.insert(std::make_pair("method", &Location::methods));
	func_map.insert(std::make_pair("redirect", &Location::redirect));
	location_pars();
}	

Location::Location(const Location & obj) : StringUtils() ,
	_error_massage("Configuration Error:\nAn invalid line was detected in the location block of the configuration file: > ")
{
	this->_methods = obj._methods;
	this->_autoIndex = obj._autoIndex;
	this->_root = obj._root;
	this->_redirection = obj._redirection;
	this->_index = obj._index;
	this->_location = obj._location;
	this->func_map = obj.func_map;
}

Location & Location::operator = (const Location & obj)
{
	if (this != & obj)
	{
		this->_methods = obj._methods;
		this->_autoIndex = obj._autoIndex;
		this->_root = obj._root;
		this->_redirection = obj._redirection;
		this->_index = obj._index;
		this->_location = obj._location;
		this->func_map = obj.func_map;
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

void	Location::location_pars()
{
	std::string key, value;

	for (size_t i = 0; i < _config.size(); i++)
	{
		size_t pos = _config[i].find(" ");
		if (pos != std::string::npos)
		{
			key = trim(_config[i].substr(0, pos), " ");
			value = trim(_config[i].substr(pos, _config[i].size()), " ");
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
	else if (str == "off" || str.empty())
		this->_autoIndex = false;
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
			_methods.insert(std::make_pair(temp[i], true));
		else if (temp[i] == "POST")
			_methods.insert(std::make_pair(temp[i], true));
		else if (temp[i] == "DELETE")
			_methods.insert(std::make_pair(temp[i], true));
		else
			throw std::runtime_error(_error_massage + str);
	}
}

bool	Location::get_method(const std::string & method)
{
	if (_methods.find(method) == _methods.end())
		return false;
	return true;
}

void Location::callFunctionByName(const std::string & fun_name, std::string & arg)
{
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
