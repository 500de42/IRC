#include "../../includes/Serveur.hpp"
#include "../../includes/Client.hpp"

Client::Client(int port)
{	std::cout << "\n\ntestclient\n\n";

    std::memset(&cli_addr, 0, sizeof(cli_addr));
    this->cli_addr.sin_port = htons(port);
    this->cli_addr.sin_family = AF_INET;
    this->cli_addr.sin_addr.s_addr = inet_addr("127.0.1.0");
    cliSocket = -1;
    servSocket = -1;
    isRegistred = false;
    goodPass = false;
    std::cout << "\n\ntestclient2\n\n";

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
    this->cliSocket = nb;
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


std::map<std::string, bool> &Client::getOpMap()
{
    return channelOp;
}

void Client::setOffOpMap(std::string channelName)
{
    for(std::map<std::string, bool>::iterator i = channelOp.begin(); i != channelOp.end(); i++)
    {
        if (channelName == i->first)
            channelOp.erase(i);
    }
}

void Client::setOp(std::string name, bool mode)
{
    this->channelOp[name] = mode;
}
bool Client::getOp(std::string name)
{
    for(std::map<std::string, bool>::iterator i = channelOp.begin(); i != channelOp.end(); i++)
    {
        if (i->first == name && i->second == true)
            return i->second;
    }
    return false;
}

bool Client::getGivenPassword()
{
    return goodPass;
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

bool Client::getRegister()
{
    return isRegistred;
}

bool Client::hasBeenInvited(std::string Name)
{
    if (invite.empty())
        for(std::vector<std::string>::iterator it = invite.begin(); it != invite.end(); it++)
        {
            if (*it == Name)
                return true;
        }
    return false;
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

std::string &Client::getRealname()
{
    return realname;
}

std::vector<Server::Channel*> &Client::getChannels()
{
    return this->channels;
}

std::string &Client::getRealBuffer()
{
    return realBuffer;
}

void Client::onRegisted()
{
    this->isRegistred = true;
}

void Client::onPass()
{
    this->goodPass = true;
}
