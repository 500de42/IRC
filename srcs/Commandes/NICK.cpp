#include "../../includes/Serveur.hpp"
#include "../../includes/Client.hpp"

void NICK(Server &server, Client &client, const char *tmp)
{
    std::stringstream ss(tmp);
    std::string w;
    std::vector<std::string> words;
    std::string buffer(tmp);

    while (ss >> w)
        words.push_back(w);
    if(words.size() != 2 || prohibidedCharacter(words[1]))
    {
        server.sendMessage("461 " + client.getNickname() + " NICK :Not enough parameters\r\n", client);
        return;
    }
    if (!server.checkDoubleName(words[1].c_str()))
    {        
        server.sendMessage(":IRCSERVER 433 " + client.getNickname() + words[1] + " :Nickname is already in use.\r\n", client);
        return;
    }
    std::string host(client.getNickname() + "!" + client.getUsername() +"@127.0.0.1 : NICK ");
    client.setNickname(words[1].c_str());
    server.sendMessage(host + words[1] + "\r\n", client);
    for(std::vector<Server::Channel *>::iterator it = client.getChannels().begin(); it != client.getChannels().end(); it++)
    {
        try
        {
            Server::Channel &channel = ChannelMatch(server, (*it)->getName());
            for(std::vector<Client *>::iterator i = channel.getMembers().begin(); i != channel.getMembers().end(); i++)
            {
                if (client.getNickname() != (*i)->getNickname())
                    server.sendMessage(host + words[1] + "\r\n",*(*i));
            }
        }
        catch(std::exception &e)
        {}
    }
}

