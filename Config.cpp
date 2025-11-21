#include "Config.hpp"

Config::~Config() {}

Config::Config() : methods(), default_file(), path(), port(-1) {}

Config::Config(const std::string & file_name) :
	port(-1), body_size(-1), seting(false), location(false) 
{
	this->path = abs_Path(file_name);
	if (!path.size())
	{
		throw std::runtime_error("Can not open or find the file 404");
	}
	read_conf_file();
}

Config &  Config::operator = (const Config & obj)
{
	if (this != &obj)
	{
		this->default_file = obj.default_file;
		this->path = obj.path;
		this->port = obj.port;
		this->body_size = obj.body_size;
		this->seting = obj.seting;
		this->location = obj.location;
	}
	return *this;
}

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

void    Config::read_conf_file()
{
	std::ifstream file (this->path.c_str());
	std::vector<std::string> file_content;
	std::string buffer;

	if (!file.is_open())
		throw std::runtime_error("Can not open the file");
	while (getline(file, buffer))
	{
		buffer = Pars::trim(buffer);
		if (buffer[0] == '#' || !buffer.length())
			continue;
		file_content.push_back(buffer);
	}
	if (file_content.size() < 9)
		throw std::runtime_error("The port config is incorrect!");
	setSetings(file_content);
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
}