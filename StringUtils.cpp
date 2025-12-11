#include "StringUtils.hpp"

StringUtils::~StringUtils() {}

StringUtils::StringUtils() : _my_tayps() {
    init();
}

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

bool    StringUtils::is_end_of_request(const std::string & buffer)
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

void    StringUtils::init()
{
    std::string buffer;
    try
    {
        validate_file("conf/mime.types");
        std::ifstream file("conf/mime.types");

        while (getline(file, buffer))
        {
            buffer = trim(buffer, " ");
            if (buffer.empty()) continue;
            
            size_t  post = buffer.find(" ");
            if (post != std::string::npos)
            {
                std::string key = trim(buffer.substr(post, buffer.length()), " ");
                std::string value = trim(buffer.substr(0, post), " ");
                _my_tayps.insert(std::make_pair(key, value));
            }
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

std::string StringUtils::get_my_taype(const std::string & file_name)
{
    size_t pos = file_name.rfind('.');

    if (pos == std::string::npos)
        return "Content-Type: application/octet-stream";
    std::map<std::string, std::string>::iterator it = _my_tayps.find(file_name.substr(pos + 1));
    if (it == _my_tayps.end())
        return "Content-Type: application/octet-stream";
    return it->second;
}

std::string StringUtils::get_file_content(const std::string & file_naem)
{
    validate_file(file_naem);
	std::ifstream file(file_naem.c_str());
	std::string file_content;
	if (file.is_open())
	{
		std::ostringstream oss;
		oss << file.rdbuf();
		file_content = oss.str();
		file.close();
    }
    return file_content;
}

std::string StringUtils::int_to_string(int num)
{
    std::stringstream ss;
    ss << num;
    return   ss.str();
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

std::string StringUtils::trim (const std::string & str, const std::string & del)
{
    std::string temp = str;
    size_t      first;
    size_t      last;

    if (str.empty())
        return temp;
    first = str.find_first_not_of(del);
    if (first == std::string::npos) {
        temp.clear();
        return str;
    }
    last = str.find_last_not_of(del);
    temp = temp.substr(first, last - first + 1);
    return temp;
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

std::string StringUtils::chang_char(std::string & src, const char chr1, const char chr2)
{
    for (size_t i = 0; i < src.size(); i++)
    {
        if (src[i] == chr1)
            src[i] = chr2;
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
        {
            temp = trim(temp, " \t");
            src = trim(src, del);
        }
        dest.push_back(temp);
    }
    if (src.length())
    {
        if (val)
            src = trim(src, " \t");
        dest.push_back(src);
    }
        
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
    file.close();
}