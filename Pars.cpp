#include "Pars.hpp"

bool    Pars::is_digitS(const std::string & str)
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

bool    Pars::recieve(const std::string & buffer)
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

std::vector<std::string> Pars::trimArray(std::vector<std::string> & array)
{
    for (size_t i = 0; i < array.size(); i++)
    {
        array[i] = trim(array[i]);
    }
    return array;
}

int	Pars::str_to_int(std::string & str)
{
	std::stringstream	ss;
	int					res;

	if (!str.size())
		return -1;
	ss << str;
	ss >> res;
	return res;	
}

std::string Pars::trim (std::string & src)
{
    if (!src.length()) return src;
	int	start = 0;
	int	end   = src.length() - 1;

	while (src[start] && (src[start] == ' ' || src[start] == '\t'))
		start++;
	while (src[end] && (src[end] == ' ' || src[end] == '\t'))
		end--;
	return (src.substr(start, end - start + 1));
}

std::string Pars::chang_char(std::string & src, const char chr, const char del)
{
    for (size_t i = 0; i < src.size(); i++)
    {
        if (src[i] == del)
            src[i] = chr;
    }
    return src;
}

std::vector<std::string> Pars::split(std::string & src, const std::string & del)
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
        dest.push_back(temp);
    }
    if (src.length())
        dest.push_back(src);
    return dest;
}

