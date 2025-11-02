#include "../../includes/Serveur.hpp"
#include "../../includes/Client.hpp"

Server::Server(char **av)
{
    std::string p(av[2]);
    srv = -1;
    this->setPassword(p);
    std::cout << "pass " << this->pass << " " << "\n";
	this->setPort(std::atoi(av[1]));
}

int Server::createServer()
{
    this->srv = socket(AF_INET, SOCK_STREAM, 0);
    if (this->srv < 0)
        return 1;
    int opt = 1;
    std::memset(&serv_addr, 0, sizeof(serv_addr));
    this->serv_addr.sin_port = htons(port);
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
    struct pollfd server_poll;
    server_poll.fd = srv;
    server_poll.events = POLLIN;
    this->fds.push_back(server_poll);
    return 0;
}


std::vector<Client*> &Server::getClients()
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
        if(this->getClients()[i]->getSocket() > 0)
            close(this->getClients()[i]->getSocket());
    }
    if (this->srv)
        close(this->srv);
}

bool Server::checkDoubleName(const char *name)
{
    if (!name)
        return false;
    for(size_t i = 0; i < this->getClients().size(); i++)
    {
        if (name == this->getClients()[i]->getUsername())
            return false;
    }
    return true;
}

void Server::sendMessage(std::string buffer, Client client)
{
    if (send(client.getSocket(), buffer.c_str(), buffer.size(), 0) == -1)
    {
        std::cout << "Error sending message:" << strerror(errno) << "\n";
    }
}

std::vector<Server::Channel *> &Server::getChannels()
{
    return channels;
}

std::vector<pollfd> &Server::getFds()
{
    return fds;
}

void Server::setPassword(std::string &name)
{
    pass = name;
}

void Server::setPort(int Port)
{
    port = Port;
}

int &Server::getPort()
{
    return port;
}

std::string &Server::getPass()
{
    return pass;
}
