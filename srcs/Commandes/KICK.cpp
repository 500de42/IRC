#include "../../includes/Serveur.hpp"
#include "../../includes/Client.hpp"
#include "../../includes/Channel.hpp"


void    KICK(Server &server, Client &client, const char *tmp)
{
    std::string buff(tmp);
    std::stringstream ss(tmp);
    std::string w;
    std::vector<std::string> words;

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
        try
        {
            Server::Channel &channel = ChannelMatch(server, words[0]);
            
            if (!searchMembers(client.getNickname(), channel))
            {
                server.sendMessage("442 " + client.getNickname() + " KICK :You are not channel member\r\n", client);
                return;
            }
            if (client.getOp(channel.getName()))
            {
                server.sendMessage("482 " + client.getNickname() + " KICK :You not channel operator\r\n", client);
                return;
            }
            if (!searchMembers(words[1], channel))
            {
                server.sendMessage("441 " + client.getNickname() + " KICK :The target is not channel member\r\n", client);
                return;
            }
            if (!searchMembers(words[1], channel))
            {
                server.sendMessage("482 " + client.getNickname() + " KICK :You not channel operator\r\n", client);
                return;
            }
            try
            {
                Client &target = clientMatch(words[1], server);

                if (!searchMembers(target.getNickname(), channel))
                {
                    server.sendMessage("441 " + client.getNickname() + " KICK :The target is not channel member\r\n", client);
                    return;
                }
                if (target.getOp(channel.getName()))
                {
                    server.sendMessage("482 " + client.getNickname() + " KICK :The target is channel operator, you can't remove it\r\n", client);
                    return;
                }
                execKick(channel, words, target, server);
            }
            catch(const std::exception& e)
            {
                server.sendMessage("403 " + client.getNickname() + " KICK :Client not found\r\n", client);
            }
        }
        catch(const std::exception& e)
        {
            server.sendMessage("403 " + client.getNickname() + " KICK :Channel don't exist\r\n", client);
        }
    }
}

void execKick(Server::Channel &channel, std::vector<std::string> words, Client &target, Server server)
{
    removeChannelMember(channel, target);
    sendMessageAllClientKick(server, words);
}
