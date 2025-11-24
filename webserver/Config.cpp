#include "Config.hpp"

Config::~Config() {
	
}

Config::Config()
	: StringUtils(), file_content(""), valid(false),  conf_seting() {}

Config::Config(std::string & strim)
	: StringUtils(), file_content(strim), valid(true), conf_seting()
{
	call_member("read_conf", NULL);
}

Config::Config(const Config & obj)	
	: StringUtils(), valid(true), conf_location(obj.conf_location), conf_seting(obj.conf_seting)
{
	this->valid = obj.valid;
	this->serv_name = obj.serv_name;
	this->error_404 = obj.error_404;
	this->error_500 = obj.error_500;
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

void Config::call_member(const std::string & fun_name, const char * arg)
{
	std::string fun_str_list[5] = { "listen", "client_max_body_size", "server_name", "error_page_404", "error_page_500" };
	std::string fun_void_list[2] = { "init_sockadd_struct","read_conf" };
	
	void (Config::*fun_str_ref[5]) (std::string &) =
	{
		&Config::listen, 
		&Config::client_max_body_size, 
		&Config::server_name,
		&Config::error_page_404,
		&Config::error_page_500
	};
	void (Config::*fun_void_ref[2]) (void) =
	{
		&Config::init_sockadd_struct,
		&Config::read_conf
	};
	try
	{
		for (size_t i = 0; i < (arg ? fun_str_list->size() : fun_void_list->size()); i++)
		{
			if ((arg ? fun_str_list[i] : fun_void_list[i]) == fun_name)
			{
				if (arg)
				{
					std::string temp = arg;
					(this->*fun_str_ref[i])(temp);
					return;
				}
				(this->*fun_void_ref[i])();
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

void	Config::listen(std::string & str) 
{
	if (!is_digitS(str))
		throw std::runtime_error("prot is  invalid");
	conf_seting.insert(std::make_pair("port", str_to_int(str)));
}
	
void	Config::client_max_body_size(std::string & str) {
	if (!is_digitS(str))
		throw std::runtime_error("max_body_size is invalid");
	conf_seting.insert(std::make_pair("max_size", str_to_int(str)));	
}

void	Config::server_name(std::string & str) {
	if (!str.size())
		throw std::runtime_error("server name is invalid");
	this->serv_name = str;
}
	
void	Config::error_page_404(std::string & str) {
	error_404 = str;
}

void	Config::error_page_500(std::string & str)
{
	error_500 = str;
}

void	Config::control() {
	if (!conf_location.size() || !conf_seting.size())
		this->valid = false;
	if (!serv_name.size())
		this->valid = false;
	if(!error_404.size() || !error_500.size())
		this->valid = false;
}

void    Config::read_conf()
{
	std::vector<std::string> array = split(file_content, "\n", true);
	
	for (size_t i = 0; i < array.size(); i++)
	{
		std::vector<std::string> conf_line = split(array[i], "=", true);
		if(conf_line.size() == 2 && conf_line[0].size() && conf_line[1].size())
		{
			if (conf_line[0] == "location")
			{
				array[i] = ("location = " + array[i]);
				try {
					Location temp(array, i);
					if (!temp.valid)
						throw std::runtime_error("Location is invalid");
					this->conf_location.push_back(Location(temp));
				}
				catch(const std::exception& e) {
					std::cerr << e.what() << '\n';
					this->valid = false;
				}
			}
			call_member(conf_line[0], conf_line[1].c_str());
			conf_line.clear();
		}
	}
	control();
}