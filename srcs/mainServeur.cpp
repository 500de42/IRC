#include "../includes/Serveur.hpp"
#include "../includes/Client.hpp"

int main()
{
    Server server;

    std::string line;
    std::vector<int> ls;

    if (server.createServer())
        return 1;
    while (1)
    {
        poll(&server.getFds()[0], server.getFds().size(), -1);
        std::getline(std::cin , line);
        if (line.empty())
            continue;
        if (server.getFds()[0].events && POLLIN)
        {
            Client client;
            server.getClients().push_back(client);
            client.setServsocket(connect(client.getSocket(), (struct sockaddr *) &server.getSockaddr(), sizeof(server.getSockaddr())));
            if (client.getServsocket() < 0)
            {
                server.closeAllSockets();
            }
        }
        for (int i = 0; i <= 10; i++)
        {
            if (server.getFds()[i].revents && POLLIN)
            {
                //parsing
            }
            else if (server.getFds()[i].revents && (POLLHUP || POLLERR))
            {
                
            }
        }
    }
    return 0;
}
