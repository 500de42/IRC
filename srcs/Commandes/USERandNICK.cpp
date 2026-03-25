#include "../../includes/Serveur.hpp"
#include "../../includes/Client.hpp"


void setUserAndNick(Client &client, Server &server, char *buff)
{
    std::string line;
    std::vector<std::string> mess;
    if (!strncmp(buff, "USER ", 5))
    {
        if (client.getUsername().empty())          
        {
            if (!extractAndSetMessageForUser(buff, client, server))
            {
                return;
            }
        }
        else
        {
            server.sendMessage("\r\n", client);//erreur a ecrire et envoyer au client
            return;
        }
    }
    else if (!strncmp(buff, "NICK ", 5))
    {
        line = extractMessage(buff + 4);
        if (!line.empty() && !prohibidedCharacter(line))
        {
            if (server.checkDoubleName(line.c_str()))
                client.setNickname(line.c_str());
            else
            {
                server.sendMessage("462" + line + ":You may not reregister\r\n", client);
                return ;
            }
        }
        else
        {
            server.sendMessage("432" + line + ":Erroneous nickname\r\n", client);
            return;
        }
    }
}

bool extractAndSetMessageForUser(char *tmp, Client &client, Server &server)
{
    if (!tmp)
        return false;

    std::stringstream ss(tmp);
    std::string l;
    std::vector<std::string> tab;

    while (ss >> l)
        tab.push_back(l);
    if (tab.size() < 4)
    {
        server.sendMessage("461 :Not enough parameters\r\n", client);   
        return false;
    }
    
    if (tab.size() > 4)
    {
        if (tab[4][0] != ':')
            return false;
        tab[4].erase(0, 1);
        std::vector<std::string> vec(tab.begin() + 4, tab.end());
        l = joinVector(vec, ' ');
        tab[4] = l;
    }
    if (prohibidedCharacter(tab[1]))
    {
        server.sendMessage("461 " + tab[1] + ":Not enough parameters\r\n", client);   
        return false;
    }
    if (client.getUsername().empty())
    {
        client.setUsername(tab[1].c_str());
        if (tab.size() > 4)
            client.setRealname(tab[4].c_str());
    }
    else
    {
        server.sendMessage("462" + tab[1] + ":You may not reregister\r\n", client);
        return false;
    }
    return (true);
}
