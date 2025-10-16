#include "../includes/Serveur.hpp"
#include "../includes/Client.hpp"

int main()
{
    Server server;
    std::string line;
    int checkPoll = -1;

    if (server.createServer())
        return 1;
    while (1)
    {
        if (poll(&server.getFds()[0], server.getFds().size(), -1) > 0)
            if (server.getFds()[0].revents & POLLIN)
            {
                Client client(false);
                client.setCliSocket(accept(server.getFds()[0].fd, NULL, NULL));
                if (client.getSocket() > 0)
                {
                    struct pollfd server_poll;
                    server_poll.fd = client.getSocket();
                    server_poll.events = POLLIN;
                    server.getClients().push_back(client);
                    server.getFds().push_back(server_poll);
                }
            }
        if (server.getFds().size() > 1)
            for (int i = 1; i <= server.getFds().size() ; i++)
            {
                size_t len = sizeof(server.getClients()[i].getBuffer());
                if (server.getFds()[i].revents & POLLIN)
                {
                    if (!recv(server.getFds()[i].fd, server.getClients()[i].getBuffer(), len, 0))
                    {
                        char *buff = server.getClients()[i].getBuffer();
                        if (buff == "KICK")
                        {

                        }
                        else if (buff == "INVITE")
                        {

                        }
                        else if (buff == "TOPIC")
                        {

                        }
                        else if (buff == "MODE")
                        {

                        }
                    }
                    else
                    {
                        // errno();
                    }
                }
                else if (server.getFds()[i].revents & POLLERR)
                {
                    
                }
                else if (server.getFds()[i].revents & POLLHUP)
                {

                }
            }
    }
    return 0;
}
