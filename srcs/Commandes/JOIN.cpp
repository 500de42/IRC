#include "../../includes/Serveur.hpp"
#include "../../includes/Client.hpp"

void extractAndSetJoin(Client &client, Server &server, std::string tmp)
{
    std::stringstream ss(tmp);
    std::vector<std::string> word;
    std::string w;

    while (ss >> w)
        word.push_back(w);
    if (word.empty() || word.size() > 2)
    {
        server.sendMessage("461 " + client.getNickname() + " JOIN :Too many parameters\r\n", client);
        return ;
    }
    if  (word[0].empty() || word[0].size() < 2)
    {
        server.sendMessage("461 " + client.getNickname() + " JOIN :Not enough parameters\r\n", client);
        return ;
    }
    if (word.size() == 1 && word[0].size() == 1 && word[0][0] == '0')
        client.quitAllChannels(server);
    std::vector<std::string> channelName = splitCommand(word[0], ',');
    std::vector<std::string> passwordCommand;
    if(word.size() == 2)
    {
        passwordCommand = splitCommand(word[1], ',');
        if (passwordCommand.size() > channelName.size())
        {
            server.sendMessage("461 " + client.getNickname() + " JOIN :Invalid channel/key list\r\n", client);
            return ;
        }
    }
    
    for (size_t i = 0; i < channelName.size(); i++)
    { 
        std::string host(client.getNickname() + "!" + client.getUsername() +"@127.0.0.1 : JOIN " + channelName[i]); 
        if(prohibidedCharacterJoin(channelName[i], true))
        {
            server.sendMessage("479 " + client.getNickname() + " " + word[0] + " :Bad Channel Mask\r\n", client);
        }
        else
        {
            channelName[i].erase(0, 1);
            if (server.getChannels().empty() || !searchChannelMatch(server, channelName[i]))
            {
                Server::Channel *channel = new(Server::Channel);
                
                channel->setOperator(client);
                channel->setName(channelName[i]);
                client.getChannels().push_back(channel);
                client.getOpMap()[channelName[i]] = true;
                channel->getMembers().push_back(&client);
                server.getChannels().push_back(channel);
                welcomeMessage(server, client);
                sendMessageAllClientJoin(client, server, *channel, host);
            }
            else
            {
                try
                {
                    Server::Channel &channel = ChannelMatch(server, channelName[i]);

                    if (searchMembers(client.getNickname(), channel))
                        continue;
                    if (channel.getK())
                    {
                        if (word.size() == 2 &&  i < passwordCommand.size() && channel.getPass() == passwordCommand[i])
                        {
                            if (channel.getL() && channel.getMembers().size() + 1 > channel.getMembersLimit())
                            {
                                server.sendMessage("475 " + client.getNickname() + " " + tmp + " :Limit reached, you cannot join channel (+l)\r\n", client);
                                continue;
                            }
                            else if (channel.getI() && !channel.hasBeenInvited(client.getNickname()))
                            {
                                server.sendMessage("475 " + client.getNickname() + " " + tmp + " :You cannot join channel without invitation (+i)\r\n", client);
                                continue;
                            }
                            server.addClientInChannel(channel, client);
                            sendMessageAllClientJoin(client, server, channel, host);
                        }
                        else
                        {
                            server.sendMessage("475 " + client.getNickname() + " " + channelName[i] + " :Bad password, you cannot join channel (+k)\r\n", client);
                        }
                    }
                    else if (channel.getL())
                    {
                        if(channel.getMembers().size() + 1 <= channel.getMembersLimit())
                        {
                            server.addClientInChannel(channel, client);
                            sendMessageAllClientJoin(client, server, channel, host);
                        }
                        else
                        {
                            server.sendMessage("475 " + client.getNickname() + " " + tmp + " :Limit reached, you cannot join channel (+l)\r\n", client);
                            continue;
                        }
                    }
                    else if (channel.getI())
                    {
                        if (channel.hasBeenInvited(client.getNickname()))
                        {
                            server.addClientInChannel(channel, client);
                            sendMessageAllClientJoin(client, server, channel, host);
                        }
                        else
                            server.sendMessage("475 " + client.getNickname() + " " + tmp + " :Cannot join channel (+i)\r\n", client);
                    }
                    else
                    {
                        server.addClientInChannel(channel, client);
                        sendMessageAllClientJoin(client, server, channel, host);
                    }
                }
                catch(std::exception &e) 
                {
                    server.sendMessage("403 " + client.getNickname() + " INVITE :Channel(" + channelName[i] + ") don't exist\r\n", client);
                }
            }
        }
    }
}

void JOIN(Client &client, Server &server, const char *tmp)
{
    std::string buffer(tmp);

    if (buffer.size() < 2)
    {
        server.sendMessage("461 " + client.getNickname() + " JOIN :Not enough parameters\r\n", client);
        return ;
    }
    extractAndSetJoin(client, server, buffer);// envoyer a l espace apres join //
    if (server.getChannels().size())
        for (size_t i = 0; i < server.getChannels().size(); i++)
        {
        }
}
