#include "../../includes/Serveur.hpp"
#include "../../includes/Client.hpp"

void execInvite(Client &client, Server &server, std::string tmp);

void INVITE(Client &client, Server &server, const char *tmp)
{
    std::string buffer(tmp);

    if (buffer.size() < 3)
    {
        server.sendMessage("461 " + client.getNickname() + " INVITE :Not enough parameters\r\n", client);
        return ;
    }
    buffer.erase(0,6);
    std::cout << "buffer invite: " << buffer << std::endl;
    execInvite(client, server, buffer);
}


void execInvite(Client &client, Server &server, std::string tmp)
{
    std::stringstream ss(tmp);
    std::vector<std::string> words;
    std::string w;

    std::cout << "INVITE 1" << std::endl;
    while (ss >> w)
        words.push_back(w);
    if (words.empty() || words.size() != 2 || words[1].size() < 2 || *words[1].begin() != '#')
    {
        server.sendMessage("461 " + client.getNickname() + " INVITE :Too many parameters\r\n", client);
        return ;
    }
    words[1].erase(0, 1);
    std::cout << "invite : " << words[0] << " " << words[1]<< std::endl;
    try
    {
        Server::Channel &channel = ChannelMatch(server, words[1]);
            
        if (!searchMembers(client.getNickname(), channel))
        {
            server.sendMessage("442 " + client.getNickname() + " INVITE :You are not channel member\r\n", client);
            return;
        }
        if (channel.getI() && !client.getOp(channel.getName()))
        {
            server.sendMessage("443 " + client.getNickname() + " INVITE :You not channel operator\r\n", client);
            return;
        }
    
        try
        {
            Client &target = clientMatch(words[0], server);
        
            if (searchMembers(target.getNickname(), channel))
            {
                server.sendMessage("441 " + client.getNickname() + " INVITE :The target is already channel member\r\n", client);
                return;
            }
            std::string host(client.getNickname() + "!" + client.getUsername() +"@127.0.0.1 : INVITE ");
            server.sendMessage("IRCSERVER.127.0.0.1 341 " + target.getNickname() + " " + client.getNickname() + " #" + words[1] + "\r\n", client);
            server.sendMessage(host + client.getNickname() + " #" + words[1] + "\r\n", target);
            channel.getInvitedMembers().push_back(target.getNickname());
        }
        catch(std::exception &e)
        {
            server.sendMessage("401 " + client.getNickname() + " INVITE :The target(" + words[0] + ") don't exist\r\n", client);
        }
    }
    catch(std::exception &e)
    {
        server.sendMessage("403 " + client.getNickname() + " INVITE :Channel(" + words[1] + ") don't exist\r\n", client);
    }
}

