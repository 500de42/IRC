#include "../includes/Serveur.hpp"
#include "../includes/Client.hpp"

bool prohibidedCharacterJoin(std::string tmp)
{
    for (std::string::iterator i = tmp.begin(); i != tmp.end(); i++)
    {
        if (*i == ' ' || *i == ',' || *i == '*')
            return true;
    }
    return false;
}

bool extractJoin(Client &client, Server &server, std::string tmp)
{
    std::stringstream ss(tmp);
    std::vector<std::string> word;
    std::string w;
    
    while (ss >> w)
        word.push_back(w);
    if (word.size() == 2)
    {
        if (word[1] == "0")
        {
            //quitter tout les channel pour ce client
        }
        if ((word[1][0] == '#' ||  word[1][0] == '&') && word[1].size() > 1)
        {
            word[1].erase(0, 5);
            if (!prohibidedCharacterJoin(word[1]))
            {
                client
            }
            else
            {}
        } 
    }
    else if (word.size() == 3)
    {

    }
    if (word.size() > 3)
    {

    }
}


void JOIN(Client &client, Server &server, const char *tmp)
{
    std::string buffer(tmp);

    if (buffer.size() < 2)
    {
        server.sendMessage("", client);
        return ;
    }
    if (server.getChannels().empty())
    {
        Server::Channel channel;
        server.getChannels().push_back(channel);
        server.getChannels().back().getMembers().push_back(client);
        server.getChannels().back().setFirst(client);

    }
    else
    {

    }
}
