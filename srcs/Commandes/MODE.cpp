#include "../includes/Serveur.hpp"
#include "../includes/Client.hpp"

void MODE(Server &server, Client &client, const char *tmp)
{
    std::string buffer(tmp);

    if (buffer.size() < 2)
    {
        server.sendMessage("461 " + client.getNickname() + " MODE :Not enough parameters\r\n", client);
        return ;
    }

}

void extractAndSetMode(Client &client, Server &server, std::string tmp)
{
    std::stringstream ss(tmp);
    std::vector<std::string> word;
    std::string w;
    int nbPassword = -1;
    
    while (ss >> w)
        word.push_back(w);
    if (word.size() > 1 || word.empty())
    {
        server.sendMessage("461 " + client.getNickname() + " MODE :Too many parameters\r\n", client);
        return ;
    }
    if (word[0][0] != '#')
    {
        server.sendMessage("", client);
        return;
    }
    try{
        Server::Channel &channel = ChannelMatch(server, word[0]);


    }
    catch(std::exception &e)
    {
        server.sendMessage("403 " + client.getNickname() + " MODE :Channel does not exist\r\n", client);
        return;
    }
}

void setModeOnChannel(std::vector<std::string> word, Client &client, Server &server)
{
    std::map<char, bool> option;
    int nbParameters = word.size() - 2;

    if (*word[1].begin() != '+' || *word[1].begin() != '-')
    {//message a bien set
        server.sendMessage("403 " + client.getNickname() + " MODE :Channel does not exist\r\n", client);
        return;
    }
    if (prohibidedCharacterMode(word[1], false))
    {
        server.sendMessage("461 " + client.getNickname() + " MODE :Not enough parameters\r\n", client);
        return;   
    }
    for (std::string::iterator i = word[1].begin(); i != word[1].end(); i++)
    {
        if (*i == '+')
        {
            if ((i + 1) != word[1].end() && checkOptions(*(i + 1)))
            {
                char next = *(i + 1);
                option[next] = true;
            }
            else
            {
                server.sendMessage("461 " + client.getNickname() + " MODE :Not enough parameters\r\n", client);
            }
        }
        else if (*i == '-')
        { 
            if ((i + 1) != word[1].end() && checkOptions(*(i + 1)))
            {
                char next = *(i + 1);
                option[next] = false;
            }
            else
            {
                server.sendMessage("461 " + client.getNickname() + " MODE :Not enough parameters\r\n", client);
            }
        }
    }
    for(std::map<char, bool>::iterator i = option.begin(); i != option.end(); i++)
    {
        
    }

}

bool prohibidedCharacterMode(std::string tmp, bool checkFirst)
{
    if (checkFirst && tmp[0] != '#')
        return true;
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
