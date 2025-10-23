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
                Client client;
                client.setCliSocket(accept(server.getFds()[0].fd, NULL, NULL));
                if (client.getSocket() > 0) 
                {
                    struct pollfd server_poll;
                    server_poll.fd = client.getSocket();
                    server_poll.events = POLLIN;
                    server.getClients().push_back(&client);
                    server.getFds().push_back(server_poll);
                    std::cout << "[Server] New client connected (fd = " << server_poll.fd << ")\n";
                }
            }
        for (int i = 1; i < server.getFds().size() ; i++)
        {
            size_t len = sizeof(server.getClients()[i]->getBuffer());
            if (server.getFds()[i].revents & POLLIN)
            {
                ssize_t bytes = recv(server.getFds()[i].fd, server.getClients()[i]->getBuffer(), len, 0);
                if (bytes > 0)
                {
                    char *buff = server.getClients()[i]->getBuffer();
                    Client &tmp = *server.getClients()[i];
                    if (!tmp.getRegister())
                    {
                        setUserAndNick(tmp, server);
                    }
                    else
                    {//METTRE EN PLACE POINTEUR SUR METHODES
                        if (!strncmp(buff, "JOIN ", 5))
                            JOIN(tmp, server, buff);
                        else if (!strncmp(buff, "KICK ", 5))
                        {

                        }
                        else if (!strncmp(buff, "INVITE ", 7))
                        {

                        }
                        else if (!strncmp(buff, "TOPIC ", 6))
                        {

                        }
                        else if (!strncmp(buff, "MODE ", 5))
                        {

                        }
                        else if (!strncmp(buff, "USER ", 5))
                        {
                            server.sendMessage("462" + (std::string)buff + ":You may not reregister\r\n", tmp);
                        }
                        else if (!strncmp(buff, "NICK ", 5))
                        {

                        }
                        else
                        {

                        }
                    }
                }
                else if (bytes == 0)
                {
                    // errno(); client deco
                }
                else 
                {
                    //error
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
