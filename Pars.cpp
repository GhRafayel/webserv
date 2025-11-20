#include "Pars.hpp"

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
    if (src.length() < 4) return dest;

    while ((pos = src.find(del)) != std::string::npos)
    {
        temp = src.substr(0, pos);
        src = src.substr(pos + del.size(), src.length());
        dest.push_back(temp);
    }
    if (src.length())
        dest.push_back(temp);
    return dest;
}

