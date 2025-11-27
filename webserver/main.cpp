
#include "Config.hpp"

volatile bool g_running = true;

static void signal_handler(int) {
	g_running = false;
}

//int main(int argc, char **argv)
int main()
{
	int argc = 2;
	char argv[2][50] = {"./webserv", "conf/server.conf"};
	Config * config = NULL;

	std::signal(SIGINT, signal_handler);
	std::signal(SIGTERM, signal_handler);
	if (argc < 3)
	{
		if (argc == 1)
			config = new Config;
		else
		{
			config = new Config(argv[1]);
			config->start();
		}
	}
	else
		std::cerr << "Error: Invalid number of parameters. Expected max : 2" << std::endl;
	delete config;
	return 0;
}