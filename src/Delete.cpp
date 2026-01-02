#include "../hpp/Delete.hpp"

Delete::~Delete () {}

Delete::Delete (Server & s_obj, Client & c_obj) : Response(s_obj, c_obj)
{
 	size_t		post = client_ref.best_mach.rfind(".");

	if (post != std::string::npos)
		ext = client_ref.best_mach.substr(post, client_ref.best_mach.size());

	// fun_map.insert(std::make_pair(200, &Delete ::fun_200)); // ok
	// fun_map.insert(std::make_pair(204, &Delete ::fun_204)); // No Content
	// fun_map.insert(std::make_pair(202, &Delete ::fun_202)); // Accepted
	// fun_map.insert(std::make_pair(400, &Delete ::fun_400));
	// fun_map.insert(std::make_pair(401, &Delete ::fun_401));
	// fun_map.insert(std::make_pair(403, &Delete ::fun_403));
	// fun_map.insert(std::make_pair(404, &Delete ::fun_404));
	// fun_map.insert(std::make_pair(500, &Delete ::fun_500));
	// fun_map.insert(std::make_pair(503, &Delete ::fun_503));

	callFunctionByStatusCode(client_ref.statuc_code);
}

void	Delete::create_response()
{
	//std::cout << client_ref.best_mach << std::endl;
}

void Delete::callFunctionByStatusCode(unsigned int fun_code)
{
	std::map<int, void (Delete::*) (void)>::iterator it = fun_map.find(fun_code);
	if (it == fun_map.end()) return ;
	(this->*(it->second))();
}