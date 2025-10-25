#include "../../includes/Serveur.hpp"
#include "../../includes/Client.hpp"

Server::Channel::Channel()
{
    membersLimit = 0;
    k = false; 
	l = false; 
	t = false; 
	i = false; 
	o = false;
}

void Server::Channel::setTopic(const std::string &name)
{
    this->topic = name;
}

void Server::Channel::setName(const std::string &name)
{
    this->name = name;
}

void Server::Channel::setOperator(Client &client)
{
    this->operators.push_back(&client);
}

void Server::Channel::setOffOperator(Client &client)
{
    for(size_t i = 0; i < this->operators.size(); i++)
    {
        if (client.getNickname() == this->operators[i]->getNickname())
        {
            operators.erase(operators.begin() + i);
            client.setOp("", false); // ajouter une option pour enlever exactement l index de l op dans la map
        }
    }
}

void Server::Channel::setPasssword(const std::string &name)
{
    this->Password = name;
}

void Server::Channel::setK(bool active)
{
    k = active;
}

void Server::Channel::setL(bool active)
{
    l = active;
}

void Server::Channel::setI(bool active)
{
    i = active;
}

void Server::Channel::setT(bool active)
{
    t = active;
}

void Server::Channel::setMembersLimit(size_t limit)
{
    this->membersLimit = limit;
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

size_t Server::Channel::getMembersLimit()
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
