
#include "My_server.hpp"

volatile bool g_running = true;

static void signal_handler(int) {
	g_running = false;
}

//int main(int argc, char ** argv)
int main()
{
	int argc = 2;
	char argv[2][100] = {"./webser", "server.conf"};

	if (argc < 3)
	{
		std::signal(SIGINT, signal_handler);
		std::signal(SIGTERM, signal_handler);
		try
		{
			if (argc == 1)
			{
				My_server srv;
				srv.start();
			}
			else
			{
				My_server srv(argv[1]);
				srv.start();
			}
		}
		catch(const std::exception & e)
		{
			std::cerr << e.what() << '\n';
		}
		return 0;
	}
	std::cerr << "Error: Invalid number of parameters. Expected max : 2" << std::endl;
	return 1;
}