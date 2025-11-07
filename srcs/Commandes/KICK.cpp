#include "../../includes/Serveur.hpp"
#include "../../includes/Client.hpp"
#include "../../includes/Channel.hpp"

//CHECK LES ARGUMENTS DE LA COMMANDE

void    KICK(Server &server, Client &client, const char *tmp)
{
    std::string buff(tmp);
    std::stringstream ss(tmp);
    std::string w;
    std::vector<std::string> words;
    std::vector<std::string> targetsList;

    if (buff.size() < 3)
    {
        server.sendMessage("461 " + client.getNickname() + " KICK :Not enough parameters\r\n", client);
        return;
    }
    else
    {
        while (ss >> w)
            words.push_back(w); // join les messages apres le ':'
        words.erase(words.begin());
        if (words[0][0] == '#')
            words[0].erase(0);
        else
        {  
            server.sendMessage("461 " + client.getNickname() + " KICK :Not enough parameters\r\n", client);
            return;
        }
        if (words.size() >= 3)
        {
            int pos = buff.find(':');
            if (pos)
            {
                words[2] = buff.substr(pos);
                words[2].erase(0);
            }
            else
            {
                server.sendMessage("461 " + client.getNickname() + " KICK :Not enough parameters\r\n", client);
                return;
            }
        }
        try
        {
            Server::Channel &channel = ChannelMatch(server, words[0]);
            
            if (!searchMembers(client.getNickname(), channel))
            {
                server.sendMessage("442 " + client.getNickname() + " KICK :You are not channel member\r\n", client);
                return;
            }
            if (!client.getOp(channel.getName()))
            {
                server.sendMessage("482 " + client.getNickname() + " KICK :You not channel operator\r\n", client);
                return;
            }
            targetsList[0] = words[1];
            if (words[1].find(','))
                targetsList = splitCommand(words[1], ',');
            
            for(std::vector<std::string>::iterator it = targetsList.begin(); it != targetsList.end(); it++)
            {
                try
                {
                    Client &target = clientMatch(*it, server);
                
                    if (!searchMembers(target.getNickname(), channel))
                    {
                        server.sendMessage("441 " + client.getNickname() + " KICK :The target is not channel member\r\n", client);
                       continue;
                    }
                    if (target.getOp(channel.getName()))
                    {
                        server.sendMessage("482 " + client.getNickname() + " KICK :The target is channel operator, you can't remove it\r\n", client);
                        continue;
                    }
                    execKick(channel, target);
                }
                catch(const std::exception& e)
                {
                    server.sendMessage("401 " + client.getNickname() + " KICK :The target don't exist\r\n", client);
                }
            }
            sendMessageAllClientKick(server, channel, words);
        }
        catch(const std::exception& e)
        {
            server.sendMessage("403 " + client.getNickname() + " KICK :Channel don't exist\r\n", client);
        } 
    }
}

void execKick(Server::Channel &channel, Client &target)
{
    removeChannelMember(channel, target);
    channel.setOffOperator(target);
}
