#include "Config.hpp"

Config::~Config() { }

Config::Config() : StringUtils(), file_content(""), valid(false),  conf_seting() {
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
}

Config::Config(std::string & strim): StringUtils(), file_content(strim), valid(true), conf_seting()
{
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	call_member("read_conf", NULL);
}

Config::Config(const Config & obj)	
	: StringUtils(), conf_location(obj.conf_location), conf_seting(obj.conf_seting)
{
	this->valid = obj.valid;
	this->serv_name = obj.serv_name;
	this->error_404 = obj.error_404;
	this->error_500 = obj.error_500;
	this->addr.sin_port = obj.addr.sin_port;
	this->addr.sin_family = obj.addr.sin_family;
	this->addr.sin_addr.s_addr = obj.addr.sin_addr.s_addr;
}

bool	Config::is_valid() const {
	return this->valid;
}

Config &  Config::operator = (const Config & obj)
{
	if (this != &obj)
	{
		std::vector<Location> temp_L(obj.conf_location);
		this->conf_location = temp_L;
		std::map<std::string, int> temp_M(obj.conf_seting);
		this->conf_seting = temp_M;
		this->valid = obj.valid;
		this->serv_name = obj.serv_name;
		this->error_404 = obj.error_404;
		this->error_500 = obj.error_500;
		this->addr.sin_port = obj.addr.sin_port;
		this->addr.sin_family = obj.addr.sin_family;
		this->addr.sin_addr.s_addr = obj.addr.sin_addr.s_addr;
	}
	return *this;
}

void Config::call_member(const std::string & fun_name, const char * arg)
{
	std::string fun_str_list[5] = { "listen", "client_max_body_size", "server_name", "error_page_404", "error_page_500" };

	std::string fun_void = "read_conf";

	void (Config::*fun_void_ref) (void) = &Config::read_conf;

	void (Config::*fun_str_ref[5]) (std::string &) =
	{
		&Config::listen, 
		&Config::client_max_body_size, 
		&Config::server_name,
		&Config::error_page_404,
		&Config::error_page_500
	};
	
	try
	{
		if (!arg && fun_name == fun_void)
		{
			(this->*fun_void_ref)();
			return;
		}
		for (size_t i = 0; i < fun_str_list->size(); i++)
		{
			if (fun_str_list[i] == fun_name)
			{
				std::string temp = arg;
				(this->*fun_str_ref[i])(temp);
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
		throw std::runtime_error("port is invalid");
	int res = str_to_int(str);
	if (res == -1)
		throw std::runtime_error("port is invalid");
	conf_seting.insert(std::make_pair("port", res));
	std::cout << res << std::endl;
	this->addr.sin_port = htons(res);
}
	
void	Config::client_max_body_size(std::string & str) {
	if (!is_digitS(str))
		throw std::runtime_error("max_body_size is invalid");
	int res =  str_to_int(str);
	if (res == -1)
		throw std::runtime_error("Max_body_sie is invalid");
	conf_seting.insert(std::make_pair("max_size", res));	
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