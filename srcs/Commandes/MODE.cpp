#include "../../includes/Serveur.hpp"
#include "../../includes/Client.hpp"

void extractAndSetMode(Client &client, Server &server, std::string tmp);
void setModeOnChannel(std::vector<std::string> word, Client &client, Server &server, Server::Channel &channel);
bool matchChannelMember(std::string name, Server::Channel channel);
bool checkNum(std::string tmp);
bool prohibidedCharacterModePassword(std::string word);
bool prohibidedCharacterMode(std::string tmp);
bool checkOptions(char c);

void MODE(Server &server, Client &client, const char *tmp)
{
    std::string buffer(tmp);

    if (buffer.size() < 2)
    {
        server.sendMessage("461 1" + client.getNickname() + " MODE :Not enough parameters\r\n", client);
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

    if (word.empty() || word.size() < 2 || word[0].empty() || word[0].size() < 2 || word[0][0] != '#')
    {
        server.sendMessage("461 2" + client.getNickname() + " MODE :Not enough parameters\r\n", client);
        return;
    }

    word[0].erase(0, 1);
    try
    {
        Server::Channel &channel = ChannelMatch(server, word[0]);
        if (client.getOp(channel.getName()) == false)
        {
            server.sendMessage("482 " + client.getNickname() + " #" + channel.getName() + " MODE :You're not channel operator\r\n", client);
            return;
        }
        if (matchChannelMember(client.getNickname(), channel))
        {
            server.sendMessage("442 " + client.getNickname() + " #" + channel.getName() + " MODE ::You're not on that channel\r\n", client);
            return;
        }
        setModeOnChannel(word, client, server, channel);
    }
    catch(std::exception &e)
    {
        server.sendMessage("403 1" + client.getNickname() + " MODE :Channel(" + word[0] + ") does not exist\r\n", client);
        return;
    }
}

void setModeOnChannel(std::vector<std::string> word, Client &client, Server &server, Server::Channel &channel)
{
    std::map<char, bool> option;
    std::vector<std::string> parameters;

    if (word.size() > 2)
        parameters.assign(word.begin() + 2, word.end());
    int nbParameters = word.size() - 2;
    int executedParameters = 0;
    std::cout << "nbparametre. " << nbParameters << " executedParameters. " << executedParameters  << "word: " << word[0] << " " << word[1] << " size: "<< word.size() << std::endl;
    if (prohibidedCharacterMode(word[1]) || word[1].size() < 2)
    {
        std::cout << "test caractere interdit :" << prohibidedCharacterMode(word[1]) << "  size:" << word[1].size() << "word[1]: " << word[1] << std::endl;
        server.sendMessage("461 3" + client.getNickname() + " MODE :Not enough parameters\r\n", client);
        return;   
    }
    for (std::string::iterator i = word[1].begin() + 1; i != word[1].end(); i++)
    {
        if (checkOptions(*i))
        {
            if (*word[1].begin() == '+')
                option[*i] = true;
            else
                option[*i] = false;
        }
        else
            server.sendMessage("461 4" + client.getNickname() + " MODE :Not enough parameters\r\n", client);
    }

    for(std::map<char, bool>::iterator i = option.begin(); i != option.end(); i++)
    {
        if (word.size() > 2 && !parameters.empty())
            std::cout << "index: " << *parameters.begin() << std::endl;
        if ((i->first == 'k' || i->first == 'o' || i->first == 'l' ) && i->second && parameters.empty())
        {            
            std::cout << "parameters: " << *parameters.begin() << "i->first: " << i->first << "second: " << i->second << std::endl;
            server.sendMessage("461 3" + client.getNickname() + " MODE :Not enough parameters\r\n", client);
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
                if (!prohibidedCharacterModePassword(*parameters.begin()))
                {
                    channel.setPasssword(*parameters.begin());
                    channel.setK(true);
                }
                else
                { 
                    std::cout << "parameters.begin() " << *parameters.begin() << std::endl;
                    server.sendMessage("476 " + client.getNickname() + " " + channel.getName() + " MODE :Not enough parameters\r\n" ,client);
                }
                if (word.size() > 2 && !parameters.empty())
                        parameters.erase(parameters.begin());
            }
            else
            {
                channel.setK(false);
                channel.setPasssword("");
            }
        }
        else if (i->first == 'o')
        {
            if (!matchChannelMember(*parameters.begin(), channel))
            {
                if (i->second == true)
                    channel.setOperator(client);
                else
                    channel.setOffOperator(client);
                if (word.size() > 2 && !parameters.empty())
                    parameters.erase(parameters.begin());
            }
            else
                server.sendMessage("441 " + client.getNickname() + " MODE :The client aren't in that channel\r\n", client);
        }
        else if (i->first == 'l')
        {
            if (checkNum(*parameters.begin()) && atol(((*parameters.begin()).c_str())) < 4294967295)
            {
                if (i->second == true)
                {
                    std::cout << "limite rentrer: " << atol((*parameters.begin()).c_str()) << std::endl;
                    channel.setL(true);
                    channel.setMembersLimit(atol((*parameters.begin()).c_str()));
                }
                else
                {
                    channel.setL(false);
                    channel.setMembersLimit(0);
                }
                    if (word.size() > 2 && !parameters.empty())
                        parameters.erase(parameters.begin());
            }
            else
                server.sendMessage("461 4" + client.getNickname() + " MODE :Not enough parameters\r\n", client);
        }
    }
}

bool matchChannelMember(std::string name, Server::Channel channel)
{
    for (size_t i = 0; i < channel.getMembers().size(); i++)
    {
        std::cout << "channel members: "<< channel.getMembers()[i]->getNickname() << "NAME: " << name << std::endl;
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
    size_t limit = atol(tmp.c_str());
    if (limit < 1)
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

bool prohibidedCharacterMode(std::string tmp)
{
    int check = 0;
    if (tmp[0] != '+' && tmp[0] != '-')
        return true;
    for (std::string::iterator i = tmp.begin(); i != tmp.end(); i++)
    {
        if (*i == '+' || *i == '-')
            check++;
        if (*i != 't' && *i != 'o' && *i != 'l' && *i != 'k' && *i != 'i' && *i != '+' && *i != '-')
            return true;
    }
    if (check != 1)
        return true;
    return false;
}

bool checkOptions(char c)
{
    if (c == 't' || c == 'o' || c == 'l' || c == 'k' || c == 'i')
        return true;
    return false;
}
