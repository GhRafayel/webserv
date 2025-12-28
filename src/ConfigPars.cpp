#include "../hpp/ConfigPars.hpp"

ConfigPars::~ConfigPars()
{
	_locations.clear();
	_token_nl.clear();
	_is_open_location.clear();
	_key_value.clear();
	_file_content.clear();
	_methods.clear();
}

ConfigPars::ConfigPars(std::string & file_path) :
	StringUtils(),
	_is_open_http(false),
	_is_open_server(false),
	_ind(0),
	_locations(),
	_token_nl(),
	_is_open_location(),
	_key_value(),
	_error_massage("Configuration Error:\nAn invalid line was detected in the server block of the configuration file: > "),
	_methods(),
	func_map(init_fun_map())
{
	read_config_file(file_path);
}

std::map<std::string, void (ConfigPars::*) (void)> ConfigPars::init_fun_map()
{
	std::map<std::string, void (ConfigPars::*) (void)> temp;

	temp.insert(std::make_pair("http_block", &ConfigPars::http_block));
	temp.insert(std::make_pair("server_block", &ConfigPars::server_block));
	temp.insert(std::make_pair("location_block", &ConfigPars::location_block));
	temp.insert(std::make_pair("is_valid_line", &ConfigPars::is_valid_line));
	temp.insert(std::make_pair("listen", &ConfigPars::listen));
	temp.insert(std::make_pair("client_max_body_size", &ConfigPars::client_max_body_size));
	temp.insert(std::make_pair("server_name", &ConfigPars::server_name));
	temp.insert(std::make_pair("error_page_404", &ConfigPars::error_page_404));
	temp.insert(std::make_pair("error_page_500", &ConfigPars::error_page_500));
	temp.insert(std::make_pair("close_blocks", &ConfigPars::close_blocks));
	temp.insert(std::make_pair("root", &ConfigPars::root));
	temp.insert(std::make_pair("index", &ConfigPars::index));
	temp.insert(std::make_pair("method", &ConfigPars::method));
	temp.insert(std::make_pair("methods", &ConfigPars::method));
	return temp;
}

void	ConfigPars::read_config_file(std::string & file_name)
{
	std::ifstream	file(file_name.c_str());
	std::string		buffer;

	while (getline(file, buffer))
	{
		buffer = trim(buffer, " \t");
		if (buffer[0] == '#' || !buffer.length())
			continue;
		_file_content += buffer + "\n";
	}
	_token_nl = split(_file_content, "\n", true);
	while (_ind < _token_nl.size())
		callFunctionByName("is_valid_line");
}

void	ConfigPars::server_block()
{
	if (_is_open_server || !_is_open_http || !_is_open_location.empty())
		throw std::runtime_error(_error_massage + _token_nl[_ind]);
		
	if (_token_nl[_ind][_token_nl[_ind].size() - 1] != '{')
	{
		if (_ind + 1 >= _token_nl.size() || _token_nl[_ind + 1] != "{" || _token_nl[_ind][6] != '\0')
			throw std::runtime_error(_error_massage + _token_nl[_ind]);
		_ind++;
	}
	_ind  += 1;
	_is_open_server = true;
}

void	ConfigPars::join_location_path(std::string & l_block)
{
	if (check_config_blocks("location"))
	{
		std::string parent_path = trim(l_block.substr(8, l_block.size()), " ");

		std::string chaild_path = trim(_token_nl[_ind].substr(8, _token_nl[_ind].size()), " ");
		_token_nl[_ind] = "location " + parent_path + chaild_path;
	}
	else
		throw std::runtime_error(_error_massage + _token_nl[_ind]);
}

void	ConfigPars::location_block()
{
	std::vector<std::string> l_block;

	if (!_is_open_server || !_is_open_http)
		throw std::runtime_error(_error_massage + _token_nl[_ind]);

	if (_token_nl[_ind][_token_nl[_ind].size() - 1] != '{')
	{
		if (_ind + 1 >= _token_nl.size() || _token_nl[_ind + 1] != "{" )
			throw std::runtime_error(_error_massage + _token_nl[_ind]);
		l_block.push_back(_token_nl[_ind]);
		_ind++;
	}
	else
	{
		_token_nl[_ind][_token_nl[_ind].size() - 1] = ' ';
		l_block.push_back(_token_nl[_ind]);
	}
	_ind++;
	_is_open_location.push_back(true);

	while (_ind < _token_nl.size())
	{
		if (_token_nl[_ind].find("location") != std::string::npos)
		{
			join_location_path(l_block[0]);
			location_block();
		}	
		else if (_token_nl[_ind] == "}")
		{
			close_blocks();
			break;
		}
		else
		{
			l_block.push_back(_token_nl[_ind]);
			_ind++;
		}
	}
	this->_locations.push_back(Location(l_block));
}

void	ConfigPars::create_server_class()
{
	for (size_t i = 0; i < _ports.size(); i++)
	{
		Server temp;
		for (size_t i = 0; i < _locations.size(); i++)
			temp._locations.push_back(_locations[i]);
		temp._root = _root;
		temp._server_name = _server_name;
		temp._body_max_size = str_to_int(_body_max_size);
		temp._error_404 = _error_404;
		temp._error_500 = _error_500;
		temp._port = _ports[i];
		temp._index = _index;
		temp._addr.sin_port = htons(_ports[i]);
		temp._methods = _methods;
		this->servers.push_back(temp);
	}
}

void	ConfigPars::clear_all()
{
	_locations.clear();
	_ports.clear();
	_key_value.clear();
	_file_content.clear();
	_index.clear();
	_root.clear();
	_server_name.clear();
	_body_max_size.clear();
	_error_404.clear();
	_error_500.clear();
	_error_massage.clear();
	_methods.clear();
}

void	ConfigPars::close_blocks()
{
	if (!_is_open_location.empty())
	{
		_is_open_location.pop_back();
	}
	else if (_is_open_server)
	{
		_is_open_server = false;
		create_server_class();
		clear_all();
	}
	else if (_is_open_http)
	{
		if (_ind + 1 != _token_nl.size())
			throw std::runtime_error(_error_massage + _token_nl[_ind]);
	}
	else
		throw std::runtime_error(_error_massage + _token_nl[_ind]);
	_ind++;
}

void	ConfigPars::http_block()
{
	if (_is_open_http || _is_open_server || !_is_open_location.empty())
		throw std::runtime_error(_error_massage + _token_nl[_ind]);
	if (_token_nl[_ind][_token_nl[_ind].size() - 1] != '{')
	{
		if (_ind + 1 >= _token_nl.size() || _token_nl[_ind + 1] != "{" || _token_nl[_ind][4] != '\0')
			throw std::runtime_error(_error_massage + _token_nl[_ind]);
		_ind += 1;
	}
	_ind += 1;
	_is_open_http = true;
}

bool	ConfigPars::check_config_blocks(const std::string & str) {

	if (str == "http")
	{
		if (_token_nl[_ind].substr(0, 4) != "http")
			return false;
		for (size_t i = 4; i < _token_nl[_ind].size(); i++)
		{
			if (_token_nl[_ind][i] != ' ' && _token_nl[_ind][i] != '{')
				return false;
		}
	}
	else if (str == "server")
	{
		if (_token_nl[_ind].substr(0, 6) != "server")
			return false;
		for (size_t i = 6; i < _token_nl[_ind].size(); i++)
		{
			if (_token_nl[_ind][i] != ' ' && _token_nl[_ind][i] != '{')
				return false;
		}
	}
	else if (str == "location")
	{
		if (_token_nl[_ind].substr(0, 8) != "location" || trim( _token_nl[_ind].substr(8,  _token_nl[_ind].size()), " ").empty())
			return false;
	}
	return true;
}

void	ConfigPars::is_valid_line()
{
	if (check_config_blocks("http"))
		callFunctionByName("http_block");
	else if (check_config_blocks("server"))
		callFunctionByName("server_block");
	else if (check_config_blocks("location"))
		callFunctionByName("location_block");
	else if (_token_nl[_ind] == "}")
		callFunctionByName("close_blocks");
	else if (_token_nl[_ind][_token_nl[_ind].size() - 1] == ';')
	{
		_token_nl[_ind][_token_nl[_ind].size() - 1] = ' ';
		_key_value = split(_token_nl[_ind], " ", true);
		callFunctionByName(_key_value[0]);
		_key_value.clear();
		_ind++;
	}
	else
		throw std::runtime_error(_error_massage + _token_nl[_ind]);
}

void	ConfigPars::listen()
{
	if (_key_value.size() != 2 || !is_digitS(_key_value[1]))
		throw std::runtime_error(_error_massage + _token_nl[_ind]);
	_ports.push_back(str_to_int(_key_value[1]));
}

void	ConfigPars::method()
{
	if (_key_value.size() == 1 || !_key_value[1].size())
	{
		_methods.insert(std::make_pair("GET", true));
	}
	else
	{
		for (size_t i = 1; i < _key_value.size(); i++)
		{
			_methods.insert(std::make_pair(_key_value[i], true));
		}		
	}
}

void	ConfigPars::client_max_body_size() {

	if (_key_value.size() != 2 || !is_digitS(_key_value[1]))
		throw std::runtime_error(_error_massage + _token_nl[_ind]);
	this->_body_max_size = _key_value[1];
}

void	ConfigPars::server_name() {
	if (_key_value.size() != 2)
		throw std::runtime_error(_error_massage + _token_nl[_ind]);
	this->_server_name = _key_value[1];
}

void	ConfigPars::error_page_404() {
	if (_key_value.size() != 2 || _key_value[1].empty())
		throw std::runtime_error(_error_massage + _token_nl[_ind]);
	this->_error_404 = _key_value[1];
}

void	ConfigPars::error_page_500() {
	if (_key_value.size() != 2 || _key_value[1].empty())
		throw std::runtime_error(_error_massage + _token_nl[_ind]);
	this->_error_500 = _key_value[1];
}

void	ConfigPars::index()
{
	if (_key_value.size() != 2 || _key_value[1].empty())
		throw std::runtime_error(_error_massage + _token_nl[_ind]);
	this->_index = _key_value[1];
}

void	ConfigPars::root()
{
	if (_key_value.size() != 2 || _key_value[1].empty())
	{
		throw std::runtime_error(_error_massage + _token_nl[_ind]);
	}
	this->_root = _key_value[1];
}

void	ConfigPars::callFunctionByName(const std::string & fun_name)
{	
	std::map<std::string, void (ConfigPars::*) (void)>::iterator	it = func_map.find(fun_name);
	if(it == func_map.end())
		throw std::runtime_error(_error_massage + fun_name);
	(this->*func_map[it->first])();
}
