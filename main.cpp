
#include "My_server.hpp"

volatile bool g_running = true;

static void signal_handler(int) {
	g_running = false;
}

int main(int argc, char ** argv)
{
	if (argc < 3)
	{
		std::string path;
		std::signal(SIGINT, signal_handler);
		std::signal(SIGTERM, signal_handler);
		if (argc == 2)
			path = argv[1];
		try
		{
			My_server srv(path);
			srv.start();
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what() << '\n';
		}
		return 0;
	}
	std::cerr << "Error: Invalid number of parameters. Expected max : 2" << std::endl;
	return 1;
}