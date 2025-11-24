#include "Config.hpp"

Config::~Config() {}

Config::Config()
	: StringUtils(), file_content(""), valid(false),  conf_seting() {}

Config::Config(std::string & strim)
	: StringUtils(), file_content(strim), valid(true), conf_seting()
{
	call_member("read_conf");
}

Config::Config(const Config & obj)	
	: StringUtils(), valid(true), conf_seting(obj.conf_seting)
{
	this->valid = obj.valid;
}

bool	Config::is_valid() const {
	return this->valid;
}

Config &  Config::operator = (const Config & obj)
{
	if (this != &obj)
	{
		return *this;
	}
	return *this;
}

void	Config::init_sockadd_struct(){

	addr.sin_family = AF_INET;
	addr.sin_port = htons(conf_seting["port"]);
	addr.sin_addr.s_addr = INADDR_ANY;
}

void Config::call_member(const std::string & fun_name)
{
	std::string fun_list[2] = {"init_sockadd_struct", "read_conf"};
	
	void (Config::*fun_ref[2]) (void) = {&Config::init_sockadd_struct, &Config::read_conf};
	try
	{
		for (size_t i = 0; i < fun_list->size(); i++)
		{
			if (fun_list[i] == fun_name)
			{
				(this->*fun_ref[i])();
				return;
			}
		}
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		this->valid = false;
	}
}

/*
std::string Config::getPath() {
	return this->path;
}

std::string Config::getDefaultFile()
{
	return this->default_file;
}

int    Config::getPort() {
	return this->port;
}

void    Config::setPort(std::string & line) {
	
	std::vector<std::string> arr = Pars::split(line, "=");
	arr = Pars::trimArray(arr);

	if (arr.size() != 2 )
		throw std::runtime_error("The port config is incorrect!");
	if (arr[0] != "listen" || !Pars::is_digitS(arr[1]))
		throw std::runtime_error("The port config is incorrect!");
	this->port = Pars::str_to_int(arr[1]);
}

void    Config::setSize(std::string & line) {
	std::vector<std::string> arr = Pars::split(line, "=");
	arr = Pars::trimArray(arr);

	if (arr.size() != 2 || arr[0] != "client_max_body_size" || !Pars::is_digitS(arr[1]))
		throw std::runtime_error("The port config is incorrect!");
	this->body_size = Pars::str_to_int(arr[1]);
}

void	Config::setPath(std::string & line)
{
	std::vector<std::string> arr = Pars::split(line, "=");
	if (arr.size() != 2)
		throw std::runtime_error("The port config is incorrect!");
	this->path = Pars::trim(arr[1]);
}	

void	Config::setFileName(std::string & name)
{
	std::vector<std::string> arr = Pars::split(name, "=");
	if (arr.size() != 2)
		throw std::runtime_error("The port config is incorrect!");
	this->default_file = Pars::trim(arr[1]);
}

void	Config::check_otehr(const std::string & s, std::string & str)
{
	std::vector<std::string> arr = Pars::split(str, "=");
	arr = Pars::trimArray(arr);

	if (arr.size() != 2)
		throw std::runtime_error("The port config is incorrect!");
	std::string temp = arr[0] + " = " + arr[1];
	if (temp != s)
		throw std::runtime_error("The port config is incorrect!");
}

void	Config::setMethods(std::string & line)
{
	std::vector<std::string> arr = Pars::split(line, "=");
	arr = Pars::trimArray(arr);

	if (arr.size() != 2 || arr[0] != "methods")
		throw std::runtime_error("The port config is incorrect!");
	this->methods = Pars::split(arr[1], " ");
	if (!methods.size())
		throw std::runtime_error("The port config is incorrect!");
	for (size_t i = 0; i < methods.size(); i++)
	{
		methods[i] = Pars::trim(methods[i]);
		if (methods[i] != "GET" && methods[i] != "POST" && methods[i] != "DELETE")
			throw std::runtime_error("The port config is incorrect!");
	}
}

void    Config::setSetings(std::vector<std::string> & conf_file)
{
	for (size_t i = 0; i < conf_file.size(); i++)
	{
		if (conf_file[i] == "[setings]")
		{
			if (i + 4 >= conf_file.size() || this->seting || conf_file[i + 1] != "{" || conf_file[i + 4] != "}")
				throw std::runtime_error("The port config is incorrect!");
			setPort(conf_file[i + 2]);
			setSize(conf_file[i + 3]);
			this->seting = true;
			i += 4;
		}
		if (conf_file[i] == "[location /]")
		{
			if (i + 6 >= conf_file.size() || this->location)
				throw std::runtime_error("The port config is incorrect!");
			setPath(conf_file[i + 2]);
			setFileName(conf_file[i + 3]);
			check_otehr("autoindex = off", conf_file[i + 4]);
			setMethods(conf_file[i + 5]);
			this->location = true;
			i += 6;
		}
	}
	if (!this->seting || !this->location)
		throw std::runtime_error("The port config is incorrect!");
}



bool	Config::getMethods(const std::string & method) {
	for (size_t i = 0; i < this->methods.size(); i++)
	{
		if (methods[i] == method)
			return true;
	}
	return false;
}

std::string Config::abs_Path(const std::string & name)
{
	std::string path;
	char actualpath [1024];
	char *r_path = realpath(name.c_str(), actualpath);
	if (!r_path)
		return "";
	path = r_path;
	return path;
}*/


void    Config::read_conf()
{
	std::vector<std::string> array = split(file_content, "\n", true);

	for (size_t i = 0; i < array.size(); i++)
	{
		
	}
}