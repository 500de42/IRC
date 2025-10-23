#include "../includes/Serveur.hpp"
#include "../includes/Client.hpp"

void Server::Channel::setTopic(const std::string &name)
{
    this->topic = name;
}

void Server::Channel::setName(const std::string &name)
{
    this->name = name;
}

void Server::Channel::setFirst(Client &client)
{
    this->First = client;
}

void Server::Channel::setPasssword(const std::string &name)
{
    this->Password = name;
}

void Server::Channel::setK(bool &active)
{
    k = active;
}

void Server::Channel::setL(bool &active)
{
    l = active;
}

void Server::Channel::setI(bool &active)
{
    i = active;
}

void Server::Channel::setT(bool &active)
{
    t = active;
}

bool &Server::Channel::getI()
{
    return i;
}

bool &Server::Channel::getK()
{
    return k;
}

bool &Server::Channel::getT()
{
    return t;
}

bool &Server::Channel::getL()
{
    return l;
}

int Server::Channel::getMembersLimit()
{
    return membersLimit;
}

std::string &Server::Channel::getName()
{
    return this->name;
}

std::string &Server::Channel::getTopic()
{
    return this->topic;
}

std::vector<Client*> &Server::Channel::getMembers()
{
    return this->members;
}

std::string &Server::Channel::getPass()
{
    return this->Password;
}
