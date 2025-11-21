#include "Config.hpp"

Config::~Config() {}

Config::Config() :  buffer(""), path(""), port(-1) {}


Config::Config(const std::string & file_name) :  buffer(""), path(""), port(-1)
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
        this->buffer = obj.buffer;
        this->path = obj.path;
        this->port = obj.port;
    }
    return *this;
}

std::string Config::getPath() {
    return this->path;
}

int    Config::getPort() {
    return this->port;
}

void    Config::setPort(std::string & str)
{
    std::vector<std::string> cont = Pars::split(str, " ");
    if (cont.size() != 2)
        throw std::runtime_error("The port config is incorrect!");
    cont[1] = Pars::trim(cont[1]);
    if (Pars::trim(cont[0]) != "listen")
       throw std::runtime_error("The port config is incorrect!");
    size_t  pos = cont[1].find("127.0.0.1:");
    if (pos == std::string::npos || pos != 0 || cont[1].length() != 14)
        throw std::runtime_error("The port config is incorrect!");
    std::stringstream s(cont[1].substr(10, 4));
    s >> this->port;
}

void    Config::check_others(std::vector<std::string> & content) {

    size_t i = 0;
    size_t len = content.size();
    if (content[2] != "server_name localhost" ||
        content[3] != "error_page 404 /errors/404.html" ||
        content[4] != "client_max_body_size 1048576")
        {
           throw std::runtime_error("The port config is incorrect!");
        }
    while (content[i] != "location /")
        i++;
    if ((++i < len && content[i] != "root ./www") ||
        (++i < len && content[i] != "index conf/index.html") ||
        (++i < len && content[i] != "methods GET POST DELETE") ||
        (++i < len && content[i] != "autoindex off")
        )
    {
       throw std::runtime_error("The port config is incorrect!");
    }
}

void    Config::read_conf_file()
{
    std::ifstream file (this->path.c_str());
    std::vector<std::string> cont;

    if (!file.is_open())
        throw std::runtime_error("Can not open the file");
    while (getline(file, this->buffer))
    {
        buffer = Pars::trim(buffer);
        if (buffer[0] == '#' || buffer.length() < 2)
            continue;
        cont.push_back(buffer);
    }
    if (cont.size() < 9)
        throw std::runtime_error("The port config is incorrect!");
    check_others(cont);
    setPort(cont[1]);
}

std::string Config::abs_Path(const std::string & file_name)
{
    std::string path;
    char actualpath [1024];
    char *ptr = realpath(("conf/" + file_name).c_str(), actualpath);
    if (!ptr)
        return "";
    path = ptr;
    return path;
}