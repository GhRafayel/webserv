#include "../hpp/Post.hpp"

Post::~Post() {}

Post::Post(Server & s_obj, Client & c_obj) : Response(s_obj, c_obj)
{
	size_t		post = client_ref.best_mach.rfind(".");
	if (post != std::string::npos)
		ext = client_ref.best_mach.substr(post, client_ref.best_mach.size());

	fun_map.insert(std::make_pair(200, &Post::fun_200)); // ok

	// fun_map.insert(std::make_pair(201, &Post::fun_201)); // Created
	// fun_map.insert(std::make_pair(204, &Post::fun_204)); // No Content
	
	// fun_map.insert(std::make_pair(400, &Post::fun_400)); // Payload Too Large
	// fun_map.insert(std::make_pair(401, &Post::fun_401)); // Payload Too Large
	// fun_map.insert(std::make_pair(403, &Post::fun_403)); // Payload Too Large
	// fun_map.insert(std::make_pair(404, &Post::fun_404)); // Payload Too Large
	// fun_map.insert(std::make_pair(413, &Post::fun_413)); // Payload Too Large
	// fun_map.insert(std::make_pair(500, &Post::fun_500)); // Payload Too Large
	// fun_map.insert(std::make_pair(503, &Post::fun_503)); // Payload Too Large
	create_response();
}

void	Post::create_response()
{
	for (std::map<std::string, std::string>::iterator it = client_ref.request.begin(); it != client_ref.request.end(); it++)
	{
		std::cout << it->first << " == " <<   it->second << std::endl;
	}
	this->client_ref.statuc_code = 200;

	callFunctionByStatusCode(client_ref.statuc_code);
}

void	Post::fun_200()
{
	strim	<<  "HTTP/1.1 200 OK";
	client_ref.outbuf = strim.str();
}

void Post::callFunctionByStatusCode(unsigned int fun_code)
{
	std::map<int, void (Post::*) (void)>::iterator it = fun_map.find(fun_code);
	if (it == fun_map.end())
		return ;
	(this->*(it->second))();
}


