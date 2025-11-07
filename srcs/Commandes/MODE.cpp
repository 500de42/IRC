#include "../../includes/Serveur.hpp"
#include "../../includes/Client.hpp"
#include "../../includes/Channel.hpp"

void extractAndSetMode(Client &client, Server &server, std::string tmp);
void setModeOnChannel(std::vector<std::string> word, Client &client, Server &server, Server::Channel &channel);
bool matchChannelMember(std::string name, Server::Channel channel);
bool checkNum(std::string tmp);
bool prohibidedCharacterModePassword(std::string word);
bool prohibidedCharacterMode(std::string tmp, bool checkFirst);
bool checkOptions(char c);

void MODE(Server &server, Client &client, const char *tmp)
{
    std::string buffer(tmp);

    if (buffer.size() < 2)
    {
        server.sendMessage("461 " + client.getNickname() + " MODE :Not enough parameters\r\n", client);
        return ;
    }
    extractAndSetMode(client, server, buffer);
}

void extractAndSetMode(Client &client, Server &server, std::string tmp)
{
    std::stringstream ss(tmp);
    std::vector<std::string> word;
    std::string w;
    
    while (ss >> w)
        word.push_back(w);
    word.erase(word.begin());
    // if (word.size() < 1 || word.empty())
    // {
    //     server.sendMessage("461 " + client.getNickname() + " MODE :Not enough parameters\r\n", client);
    //     return ;
    // }
    if (word[0][0] != '#')
    {
        server.sendMessage("461 " + client.getNickname() + " MODE :Not enough parameters\r\n", client);
        return;
    }
    word[0].erase(0, 1);
    try{
        Server::Channel &channel = ChannelMatch(server, word[0]);
        if (client.getOp(channel.getName()) == false)
        {
            server.sendMessage("482 " + client.getNickname() + " #" + channel.getName() + " MODE :You're not channel operator\r\n", client);
            return;
        }
        setModeOnChannel(word, client, server, channel);
    }
    catch(std::exception &e)
    {
        server.sendMessage("403 1" + client.getNickname() + " MODE :Channel does not exist\r\n", client);
        return;
    }
}

void setModeOnChannel(std::vector<std::string> word, Client &client, Server &server, Server::Channel &channel)
{
    std::map<char, bool> option;
    int nbParameters = word.size() - 2;
    int executedParameters = 0;
    std::cout << "nbparametre. " << nbParameters << " executedParameters. " << executedParameters  << "word: " << word[0] << " " << word[1] << " size: "<< word.size() << std::endl;

    if (prohibidedCharacterMode(word[1], false))
    {
        server.sendMessage("461 " + client.getNickname() + " MODE :Not enough parameters\r\n", client);
        return;   
    }
    if (*word[1].begin() != '+' && *word[1].begin() != '-')//message a bien set
    {
        std::cout << "word : " << *word[1].begin() << " " << word[1] << std::endl;
        server.sendMessage("403 2" + client.getNickname() + " MODE :Not enough parameters\r\n", client);
    }
    for (std::string::iterator i = word[1].begin(); i != word[1].end(); i++)
    {
        if (*i == '+' || *i == '-')
        {
            if ((i + 1) != word[1].end() && checkOptions(*(i + 1)))
            {
                char next = *(i + 1);
                if (*i == '+')
                    option[next] = true;
                else
                    option[next] = false;
            }
            else
                server.sendMessage("461 " + client.getNickname() + " MODE :Not enough parameters\r\n", client);
        }
    }
    int index = 2;
    for(std::map<char, bool>::iterator i = option.begin(); i != option.end(); i++)
    {
        if ((i->first == 'k' || i->first == 'o' || i->first == 'l' ) && (nbParameters <= executedParameters))
        {            std::cout << "nbparametre: " << nbParameters << " executedParameters" << executedParameters  << "i->first: " << i->first << "second: " << i->second << std::endl;
            server.sendMessage("403 3" + client.getNickname() + " MODE :Not enough parameters\r\n", client);
        }
        else if (i->first == 'i')
        {
            if (i->second == true)
                channel.setI(true);
            else
                channel.setI(false);
        }
        else if (i->first == 't')
        {
            if (i->second == true)
                channel.setT(true);
            else
                channel.setT(false);
        }
        else if (i->first == 'k')
        {
            if (i->second == true)
            {
                if (!prohibidedCharacterModePassword(word[index]))
                {
                    channel.setPasssword(word[index]);
                    channel.setK(true);
                }
                else
                { std::cout << "word[index] " << word[index] << std::endl;
                    server.sendMessage("476 " + client.getNickname() + " " + channel.getName() + " MODE :Not enough parameters\r\n" ,client);
                    index++;
                    executedParameters++;
                    continue;
                }
            }
            else
            {
                channel.setK(false);
                channel.setPasssword("");
            }
        }
        else if (i->first == 'o')
        {
            if (matchChannelMember(word[index], channel))
            {
                if (i->second == true)
                {
                    channel.setOperator(client);
                    // channel.setO(true);
                }
                else
                {}//cree une fonction qui enleve les client op
            }
            else
                server.sendMessage("441 " + client.getNickname() + " MODE :Not enough parameters\r\n", client);
        }
        else if (i->first == 'l')
        {
            if (checkNum(word[index]))
            {
                if (i->second == true)
                {
                    channel.setI(true);
                    channel.setMembersLimit(atoi(word[index].c_str()));
                }
                else
                {
                    channel.setI(false);
                    channel.setMembersLimit(0);
                }
            }
            else
                server.sendMessage("403 4" + client.getNickname() + " MODE :Not enough parameters\r\n", client);
        }
        index++;
        executedParameters++;
    }

}

bool matchChannelMember(std::string name, Server::Channel channel)
{
    for (size_t i = 0; i < channel.getMembers().size(); i++)
    {
        if (name == channel.getMembers()[i]->getNickname())
            return false;
    }
    return true;
}

bool checkNum(std::string tmp)
{
    for(std::string::iterator i = tmp.begin(); i != tmp.end(); i++)
    {
        if (!isdigit(*i))
            return false;
    }
    if (atol(tmp.c_str()) < 0)
        return false;
    return true;
}

bool prohibidedCharacterModePassword(std::string word)
{
    for (std::string::iterator i = word.begin(); i != word.end(); i++)
    {
        if (*i == ',' || *i == '\n' || *i == '\r')
            return true;
    }
    return false;
}

bool prohibidedCharacterMode(std::string tmp, bool checkFirst)
{
    if (checkFirst && tmp[0] != '#')
        return true;
    tmp.erase(0, 1);
    for (std::string::iterator i = tmp.begin(); i != tmp.end(); i++)
    {
        if (*i != 't' && *i != 'o' && *i != 'l' && *i != 'k' && *i != 'i' && *i != '+' && *i != '-')
            return true;
    }
    return false;
}

bool checkOptions(char c)
{
    if (c == 't' || c == 'o' || c == 'l' || c == 'k' || c == 'i')
        return true;
    return false;
}
