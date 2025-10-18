#include "../includes/Serveur.hpp"
#include "../includes/Client.hpp"

Client::Client()
{
    std::memset(&cli_addr, 0, sizeof(cli_addr));
    this->cli_addr.sin_port = htons(6667);
    this->cli_addr.sin_family = AF_INET;
    this->cli_addr.sin_addr.s_addr = inet_addr("127.0.1.0");
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

void Client::setRealname(const char *name)
{
    realname = name;
}

void Client::setUsername(const char *name)
{
    username = name;
}

void Client::setNickname(const char *name)
{
    nickname = name;
}

void Client::setBuffer(char *buffer)
{
    Buffer = buffer;
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

Client &Client::operator=(const Client &cpy)
{
    if (this != &cpy) 
    {
        nickname = cpy.nickname;
        username = cpy.username;
        isRegistred = cpy.isRegistred;
    }
    return *this;
}

std::string &Client::getNickname()
{
    return this->nickname;
}

std::string &Client::getUsername()
{
    return this->username;
}

void Client::onRegisted()
{
    this->isRegistred = true;
}
