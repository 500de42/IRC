#include "../../includes/Serveur.hpp"
#include "../../includes/Client.hpp"
#include "../../includes/Channel.hpp"


void setUserAndNick(Client &client, Server &server, char *buff)
{
    std::string line;
    std::vector<std::string> mess;
    std::cout << "|" << buff << "| " << strlen(buff) << std::endl;
    if (!strncmp(buff, "USER ", 5))
    {
        std::cout << "test user1" << std::endl;
        if (client.getUsername().empty())          
        {
            std::cout << "test user2" << std::endl;
            if (!extractAndSetMessageForUser(buff, client, server))
            {    std::cout << "test user3" << std::endl;
                // ERR_ALREADYREGISTRED();
                // server.sendMessage("\r\n", client);//erreur a ecrire et envoyer au client
                return;
            }
        }
        else
        {
            server.sendMessage("\r\n", client);//erreur a ecrire et envoyer au client
            return;
        }
        std::cout << "test user4" << std::endl;
    }
    else if (!strncmp(buff, "NICK ", 5))
    {
        std::cout << "test nick" << std::endl;
        line = extractMessage(buff + 4);
        if (!line.empty() && !prohibidedCharacter(line))
        {std::cout << " NICKNAME : |" << line << std::endl;
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
    std::cout << "setusernick fini0\n";
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
    std::cout << "test u1 size: " << tab.size() << std::endl;
    if (tab.size() < 4)
    {
        std::cout << "test u2 size: " << tab.size() << std::endl;
        server.sendMessage("461 5:Not enough parameters\r\n", client);   
        return false;
    }
    std::cout << "test u1" << std::endl;
    
    if (tab.size() > 4)
    {
        if (tab[4][0] != ':')
            return false;
        tab[4].erase(0, 1);
        std::vector<std::string> vec(tab.begin() + 4, tab.end());
        l = joinVector(vec, ' ');
        tab[4] = l;
    }
    if (prohibidedCharacter(tab[1]) || (tab.size() > 4 && (tab[4].size() && prohibidedCharacter(tab[4]))))
    {
        std::cout << "test u2 tab 1: " << prohibidedCharacter(tab[1]) << "tab2 : " << prohibidedCharacter(tab[4]) << std::endl;
        server.sendMessage("461 22" + tab[1] + ":Not enough parameters\r\n", client);   
        return false;
    }
    if (client.getUsername().empty())
    {
        if (tab.size() > 4)
            std::cout << " USERNAME : " << tab[1] << " " << tab[4] << std::endl;
        std::cout << "USER set" << std::endl;
        client.setUsername(tab[1].c_str());
        if (tab.size() > 4)
            client.setRealname(tab[4].c_str());
    }
    else
    {
        std::cout << "test u4" << std::endl;
        server.sendMessage("462" + tab[1] + ":You may not reregister\r\n", client);
        return false;
    }
    return (true);
}
