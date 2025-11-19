
#include "My_server.hpp"

volatile bool g_running = true;

static void signal_handler(int) {
    g_running = false;
}

int main(int argc, char ** argv)
{
    (void)argc;
    (void)argv;
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);
    try
    {
        My_server srv;
        srv.start();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    return 0;
}