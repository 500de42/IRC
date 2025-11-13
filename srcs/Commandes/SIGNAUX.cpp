#include "../../includes/Serveur.hpp"
#include "../../includes/Client.hpp"

void signal_handler(int sig) 
{
    (void)sig; 
    g_running = 0;
}

void setup_signal_handlers()
{
    signal(SIGINT, signal_handler);
    signal(SIGQUIT, signal_handler);
    signal(SIGTERM, signal_handler);
}
