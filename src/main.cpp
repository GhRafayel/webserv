
#include "../hpp/My_server.hpp"

volatile bool g_running = true;

static void signal_handler(int) {
	g_running = false;
}

//int main(int argc, char **argv)
int main()
{
	int argc = 2;
	char argv[2][50] = {"./webserv", "conf/server.conf"};
	My_server * server = NULL;

	std::signal(SIGINT, signal_handler);
	std::signal(SIGTERM, signal_handler);
	if (argc < 3)
	{
		try
		{
			if (argc == 1)
			{
				server = new My_server();
				server->start_server();
			}
			else
			{
				server = new My_server(argv[1]);
				server->start_server();
			}
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what() << '\n';
		}
	}
	else
		std::cerr << "Error: Invalid number of parameters. Expected max : 2" << std::endl;
	delete server;
	return 0;
}