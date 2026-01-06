#include "../hpp/StringUtils.hpp"

StringUtils::~StringUtils() {}

StringUtils::StringUtils() : _my_tayps() {
	init();
}

bool    					StringUtils::is_digitS(const std::string & str)
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

bool    					StringUtils::is_end_of_request(const std::string & buffer)
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

void    					StringUtils::init()
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

std::string 				StringUtils::get_my_taype(const std::string & file_name)
{
	size_t pos = file_name.rfind('.');

	if (pos == std::string::npos)
		return "Content-Type: application/octet-stream";
	std::map<std::string, std::string>::iterator it = _my_tayps.find(file_name.substr(pos + 1));
	if (it == _my_tayps.end())
		return "Content-Type: application/octet-stream";
	return "Content-Type: " + it->second;
}

std::string					StringUtils::get_file_content(const std::string & file_path, size_t start, size_t len)
{
	std::string res;
	res.resize(len);

	std::ifstream file(file_path.c_str(), std::ios::binary);
	if (!file)
		return res;

	file.seekg(start);
	
	file.read(&res[0], len);

	res.resize(file.gcount());

	return res;
}

std::string 				StringUtils::get_file_content(const std::string & file_path)
{
	if (file_path.empty()) return file_path;

	validate_file(file_path);
	std::ifstream file(file_path.c_str());
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

std::string					StringUtils::int_to_string(int num)
{
	std::stringstream ss;
	ss << num;
	return   ss.str();
}

int							StringUtils::str_to_int(const std::string & str)
{
	std::stringstream	ss;
	int					res;

	if (!str.size())
		return -1;
	ss << str;
	ss >> res;
	return res;	
}

std::string 				StringUtils::trim (const std::string & str, const std::string & del)
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

std::string 				StringUtils::trim (std::string & str, const std::string & del)
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

std::string 				StringUtils::chang_char(std::string & src, const char chr1, const char chr2)
{
	for (size_t i = 0; i < src.size(); i++)
	{
		if (src[i] == chr1)
			src[i] = chr2;
	}
	return src;
}

std::vector<std::string>	StringUtils::split(std::string & src, const std::string & del, bool val)
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

std::string 				StringUtils::abs_Path(const std::string & name)
{
	std::string path;
	char actualpath [1024];
	char *r_path = realpath(name.c_str(), actualpath);
	if (!r_path)
		return "";
	path = r_path;
	return path;
}

void 						StringUtils::validate_file(const std::string & fileName) {

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

bool						StringUtils::is_directory(const std::string & path)
{
	DIR *dir = opendir(path.c_str());

	if (dir == NULL) return false;
	closedir(dir);
	return true;
}

bool 						StringUtils::readable(const std::string& path)
{
	if (access(path.c_str(), R_OK) == 0)
		return true;
	return false;
}

bool 						StringUtils::writable(const std::string& path)
{
	if (access(path.c_str(), W_OK) == 0)
		return true;
	return false;
}

bool 						StringUtils::executable(const std::string& path)
{
	if (access(path.c_str(), X_OK) == 0)
		return true;
	return false;
}

bool						StringUtils::exists(const std::string& path)
{
	if(access(path.c_str(), F_OK) == 0)
		return true;
	return false;
}

std::string 				StringUtils::get_http_date()
{
	char		buffer[100];
	std::time_t	now;
	std::tm		*gmt;

	now = std::time(NULL);
	gmt = std::gmtime(&now);

	std::strftime(buffer, sizeof(buffer), "Date: %a, %d %b %Y %H:%M:%S GMT", gmt);
	return std::string(buffer);
}

std::vector<std::string>	StringUtils::Range_pars(const std::string & req)
{
	std::string copy_req = req;
	std::vector<std::string> temp = split(copy_req, "=", true);
	copy_req = temp[1];
	temp  = split(copy_req, "-", true);
	return temp;
}

