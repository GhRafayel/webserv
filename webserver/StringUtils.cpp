#include "StringUtils.hpp"

StringUtils::~StringUtils() {}

StringUtils::StringUtils() {}

bool    StringUtils::is_digitS(const std::string & str)
{
    if (!str.size())
        return false;
    for (size_t i = 0; i < str.size(); i++)
    {
       if (!isdigit(str[i]))
			return false;
    }
    return true;
}

bool    StringUtils::recieve(const std::string & buffer)
{
    if (buffer.length() < 4)
    {
        return false;
    }
    size_t pos = buffer.find("\r\n\r\n");
    if (pos != std::string::npos)
    {
        return true;
    }
    return false;
}

int	StringUtils::str_to_int(std::string & str)
{
	std::stringstream	ss;
	int					res;

	if (!str.size())
		return -1;
	ss << str;
	ss >> res;
	return res;	
}

std::string StringUtils::trim (std::string & str, const std::string & del)
{
    if (str.empty()) return str;

    size_t first = str.find_first_not_of(del);
    if (first == std::string::npos) {
        str.clear();
        return str;
    }

    size_t last = str.find_last_not_of(del);
    str = str.substr(first, last - first + 1);
    return str;
}

std::string StringUtils::chang_char(std::string & src, const char chr, const char del)
{
    for (size_t i = 0; i < src.size(); i++)
    {
        if (src[i] == del)
            src[i] = chr;
    }
    return src;
}

std::vector<std::string> StringUtils::split(std::string & src, const std::string & del, bool val)
{
    std::vector<std::string>    dest;
    std::string                 temp;
    size_t                      pos;

    if (!src.length())
        return dest;
    while ((pos = src.find(del)) != std::string::npos)
    {
        temp = src.substr(0, pos);
        src = src.substr(pos + del.size(), src.length());
        if (val)
            temp = trim(temp, " \t");
        dest.push_back(temp);
    }
    if (src.length())
        dest.push_back(src);
    return dest;
}

std::string StringUtils::abs_Path(const std::string & name)
{
	std::string path;
	char actualpath [1024];
	char *r_path = realpath(name.c_str(), actualpath);
	if (!r_path)
		return "";
	path = r_path;
	return path;
}

void StringUtils::validate_file(const std::string & fileName) {

	std::ifstream file(fileName.c_str());

	if (!file.is_open())
	{
		if (errno == EACCES) {
			throw std::runtime_error("Permission denied: cannot open: " + fileName);
		} else if (errno == ENOENT) {
			throw std::runtime_error(fileName + "file not found: ");
		} else {
			throw std::runtime_error("Cannot open file: " + fileName);
		}
	}
}