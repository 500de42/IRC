#include "../includes/Serveur.hpp"
#include "../includes/Client.hpp"

int Server::createServer()
{
    this->srv = socket(AF_INET, SOCK_STREAM, 0);
    if (this->srv < 0)
        return 1;
    int opt = 1;
    std::memset(&serv_addr, 0, sizeof(serv_addr));
    this->serv_addr.sin_port = htons(6667);
    this->serv_addr.sin_family = AF_INET;
    this->serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    setsockopt(srv, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if (bind(srv, (struct sockaddr *)&serv_addr, sizeof(serv_addr)))
    {
        std::cerr << "Bind error" << std::endl;
        close(srv);
        return 1;
    }
    std::cout << "Bind ok" << std::endl;
    if (listen(srv, 10))
    {
        std::cerr << "Listen error" << std::endl;
        close(srv);
        return 1;
    }
    struct pollfd server_poll[10 + 1];
    server_poll[0].fd = srv;
    server_poll[0].events = POLLIN;
    this->fds.push_back(server_poll[0]);
    for (int i = 1; i <= 11; i++)
    {
        server_poll[i].fd = -1;
        this->fds.push_back(server_poll[i]);
    }
    return 0;
}


std::vector<Client> &Server::getClients()
{
    return clients;
}

struct sockaddr_in &Server::getSockaddr()
{
    return this->serv_addr;
}

void Server::closeAllSockets()
{
    for (int i = 0; i <= 11; i++)
    {
        if(this->getClients()[i].getFd() > 0)
            close(this->getClients()[i].getFd());
    }
    if (this->srv)
        close(this->srv);
}
