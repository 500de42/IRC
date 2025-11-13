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

Server::Channel::~Channel()
{
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
    for(std::vector<Client *>::iterator i = operators.begin(); i != operators.end(); i++)
    {
        if (client.getNickname() == (*i)->getNickname())
        {
            operators.erase(i);
            client.setOffOpMap(name); 
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

void Server::Channel::setO(bool active)
{
    o = active;
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

bool &Server::Channel::getO()
{
    return o;
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

std::vector<std::string> &Server::Channel::getInvitedMembers()
{
    return this->invited;
}

std::string &Server::Channel::getPass()
{
    return this->Password;
}

bool Server::Channel::hasBeenInvited(std::string Name)
{
    if (!invited.empty())
        for(std::vector<std::string>::iterator it = invited.begin(); it != invited.end(); it++)
        {
            if (*it == Name)
                return true;
        }
    return false;
}
