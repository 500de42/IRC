#include "../../includes/Serveur.hpp"
#include "../../includes/Client.hpp"


void PRIVMSG(Server &server, Client &client, const char *tmp)
{
    std::stringstream ss(tmp);
    std::string buffer(tmp);
    std::vector<std::string> words;
   std::string message;
    std::string w;

    std::cout << "PRIVMSG 1" << std::endl;
    while (ss >> w)
        words.push_back(w);
    if (words.empty() || words.size() < 2 )
    {
        server.sendMessage("461 " + client.getNickname() + " PRIVMSG :Not enough parameters\r\n", client);
        return ;
    }
    std::vector<std::string> targets = splitCommand(words[0], ',');
    int pos = buffer.find(':');
    if (pos)
    {
        message = buffer.substr(pos);
        message.erase(0, 1);
    }
    else
    {
        server.sendMessage("461 " + client.getNickname() + " PRIVMSG :Not enough parameters\r\n", client);
        return;
    }
    std::string host(client.getNickname() + "!" + client.getUsername() +"@127.0.0.1 : PRIVMSG ");
    for(std::vector<std::string>::iterator it = targets.begin(); it != targets.end(); it++)
    {
        if (*(*it).begin() == '#')
        {
            if ((*it).size() < 2)
            {
                server.sendMessage("403 " + client.getNickname() + " PRIVMSG :Channel don't exist\r\n", client);
                continue;
            }
            (*it).erase((*it).begin());
            try
            {
                Server::Channel &channel = ChannelMatch(server, *it);
                    
                if (!searchMembers(client.getNickname(), channel))
                {
                    server.sendMessage("442 " + client.getNickname() + " PRIVMSG :You are not channel member\r\n", client);
                    continue;
                }
                sendMessageAllClient(server, channel, host + " #" + channel.getName() + message);
            }
            catch(std::exception &e)
            {
                server.sendMessage("403 " + client.getNickname() + " PRIVMSG :Channel(" + *it + ") don't exist\r\n", client);
            }
        }
        else
        {
            try
            {
                Client &target = clientMatch(*it, server);
                
                server.sendMessage(host +  target.getNickname() + " " + message + "\r\n", target);
                server.sendMessage(host +  target.getNickname() + " " + message + "\r\n", client);
            }
            catch(std::exception &e)
            {
                server.sendMessage("401 " + client.getNickname() + " PRIVMSG :The target(" + *it + ") don't exist\r\n", client);
            }
        }
        
    }
}
