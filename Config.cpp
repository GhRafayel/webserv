#include "Config.hpp"

Config::~Config() {}

Config::Config(const std::string & conf_path) : path(conf_path) {
    if (this->path.length() == 0)
        this->path = "conf/default.conf";
    read_conf_file();
}

void Config::setPort(const int new_port) {
	this->port = new_port;
}

void    Config::setPath(const std::string new_path) {
    this->path = new_path;
}

int    Config::getPort() {
    return this->port;
}

void    Config::setPort(std::string & str)
{
        std::cout << str << std::endl;

    std::vector<std::string> cont = Pars::split(str, " ");

    if (cont.size() != 2)
        throw std::string("The port config is incorrect!");
    cont[0] = Pars::trim(cont[0]);
    cont[1] = Pars::trim(cont[1]);
    
    std::cout << cont[0] << std::endl;
    std::cout << cont[1] << std::endl;

    if (cont[0] != "listen")
        throw std::string("The port config is incorrect!");
    size_t  pos = cont[1].find("127.0.0.1:");
    if (pos == std::string::npos || pos != 0)
         throw std::string("The port config is incorrect!");
    // for (size_t i = 10; i < cont[1].size(); i++)
    // {
    //     if (i > 14 || !isdigit(cont[1][i]))
    //         throw std::string("The port config is incorrect!");
    // }
    // std::stringstream s(cont[1].substr(pos, cont[1].size()));
    // s >> this->port;
    // std::cout << this->port << std::endl;
}

void    Config::setPath(std::string & str) {
    (void)str;
}

void    Config::read_conf_file()
{
    std::ifstream file (this->path.c_str());
    std::vector<std::string> cont;

    if (!file.is_open())
        throw std::string ("Can not open the file");
    while (getline(file, this->buffer))
    {
        buffer = Pars::trim(buffer);
        if (buffer[0] == '#' || buffer.length() < 2)
            continue;
        cont.push_back(buffer);
    }
    setPort(cont[1]);
    // std::cout << "---------------------------" << std::endl;
    // for (size_t i = 0; i < cont.size(); i++)
    // {
    //     std::cout << cont[i] << std::endl;
    // }
    // std::cout << "---------------------------" << std::endl;
}