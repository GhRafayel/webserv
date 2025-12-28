#include "../hpp/Location.hpp"

Location::~Location() {  this->_config.clear(); }

Location::Location() : func_map(),_config(), _methods(), _autoIndex(false),
_root("/www/public/"), _index("index.html"), _location("/"),
_error_massage("Configuration Error:\nAn invalid line was detected in the location block of the configuration file: > "),
_return()
 {
	func_map.insert(std::make_pair("location", &Location::loc_location));
	func_map.insert(std::make_pair("root", &Location::loc_root));
	func_map.insert(std::make_pair("index", &Location::loc_index));
	func_map.insert(std::make_pair("autoindex", &Location::loc_autoindex));
	func_map.insert(std::make_pair("methods", &Location::loc_methods));
	func_map.insert(std::make_pair("method", &Location::loc_methods));
	func_map.insert(std::make_pair("return", &Location::loc_return));
 }

Location::Location(std::vector<std::string> & array) : func_map(),
_config(array), _methods(), _autoIndex(false),
_error_massage("Configuration Error:\nAn invalid line was detected in the location block of the configuration file: > "),
_return()
{
	func_map.insert(std::make_pair("location", &Location::loc_location));
	func_map.insert(std::make_pair("root", &Location::loc_root));
	func_map.insert(std::make_pair("index", &Location::loc_index));
	func_map.insert(std::make_pair("autoindex", &Location::loc_autoindex));
	func_map.insert(std::make_pair("methods", &Location::loc_methods));
	func_map.insert(std::make_pair("method", &Location::loc_methods));
	func_map.insert(std::make_pair("return", &Location::loc_return));
	location_pars();
}	

Location::Location(const Location & obj) : StringUtils() ,
	_error_massage("Configuration Error:\nAn invalid line was detected in the location block of the configuration file: > "),
	_return()
{
	this->_methods = obj._methods;
	this->_autoIndex = obj._autoIndex;
	this->_root = obj._root;
	this->_return = obj._return;
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
		this->_return = obj._return;
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

void    Location::loc_location(std::string & str)
{
	if (str.empty())
		throw std::runtime_error(_error_massage + str);
	this->_location = str;
}

void	Location::loc_return(std::string & str)
{
	this->_return = split(str, " ", true);
	if (_return.size() != 2 || !is_digitS(_return[0]))
		throw std::runtime_error(_error_massage + str);
}

void    Location::loc_root(std::string & str) {
	this->_root = str;
}

void    Location::loc_index(std::string & str) {
	this->_index = str;
}

void    Location::loc_autoindex(std::string & str) {

	if (str == "on")
		this->_autoIndex = true;
	else if (str == "off" || str.empty())
		this->_autoIndex = false;
	else
		throw std::runtime_error(_error_massage + str);
}

void    Location::loc_methods(std::string & str)
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
	std::map<std::string,bool>::iterator it = _methods.find(method);
	if (it == _methods.end())
		return false;
	return it->second;
}

void Location::callFunctionByName(const std::string & loc_name, std::string & arg)
{
	try
	{
		std::map<std::string, void (Location::*) (std::string &)>::iterator	it = func_map.find(loc_name);
		if (it == func_map.end())
			throw std::runtime_error(_error_massage + loc_name + " " + arg);
		if (arg.empty())
			return ;
		(this->*(it->second))(arg);
	}
	catch(const std::exception & e)
	{
		std::cerr << e.what() << '\n';
	}
}
