#include "../includes/Serveur.hpp"
#include "../includes/Client.hpp"

Client::Client(bool o)
{        
    if (o)
    {
        this->cliSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (this->cliSocket < 0)
            throw ("Error client socket\n");
        std::memset(&cli_addr, 0, sizeof(cli_addr));
        this->cli_addr.sin_port = htons(6667);
        this->cli_addr.sin_family = AF_INET;
        this->cli_addr.sin_addr.s_addr = inet_addr("127.0.1.0");
        this->setServsocket(connect(this->getSocket(), (struct sockaddr *) &this->getSockaddr(), sizeof(this->getSockaddr())));
        if (servSocket < 0)
            throw("Error socket init ");
    }
}

int Client::getServsocket()
{
    return this->servSocket;
}

void Client::setServsocket(int nb)
{
    this->servSocket = nb;
}

void Client::setCliSocket(int nb)
{
    this->servSocket = nb;
}

int Client::getSocket()
{
    return this->cliSocket;
}

struct sockaddr_in &Client::getSockaddr()
{
    return this->cli_addr;
}

char *Client::getBuffer()
{
    return Buffer;
}

// void Client::setBuffer(char *buffer)
// {
//     Buffer = buffer;
// }
