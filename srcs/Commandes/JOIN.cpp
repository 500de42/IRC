#include "../../includes/Serveur.hpp"
#include "../../includes/Client.hpp"

// 473 <nick> #private :Cannot join channel (+i)\r\n channel prive //

void extractAndSetJoin(Client &client, Server &server, std::string tmp)
{
    std::stringstream ss(tmp);
    std::vector<std::string> word;
    std::string w;

    std::cout << "JOIN 1" << std::endl;
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
    std::cout << "JOIN 2" << std::endl;
    if (word.size() == 1 && word[0].size() == 1 && word[0][0] == '0')
        client.quitAllChannels(server);
    std::cout << "JOIN 2.5" << std::endl;
    std::vector<std::string> channelName = splitCommand(word[0], ',');
    std::cout << "JOIN 2.6" << std::endl;
    std::vector<std::string> passwordCommand;
    std::cout << "JOIN 3" << std::endl;
    if(word.size() == 2)
    {
        std::cout << "JOIN 3.5" << std::endl;
        passwordCommand = splitCommand(word[1], ',');
        std::cout << "JOIN 4" << std::endl;
        if (passwordCommand.size() > channelName.size())
        {
            server.sendMessage("461 " + client.getNickname() + " JOIN :Invalid channel/key list\r\n", client);
            return ;
        }
    }
    std::cout << "JOIN 5" << std::endl;
    
    for (size_t i = 0; i < channelName.size(); i++)
    { 
        std::string host(client.getNickname() + "!" + client.getUsername() +"@127.0.0.1 : JOIN " + channelName[i]); 
        std::cout << "JOIN 5.2" << std::endl;
        if(prohibidedCharacterJoin(channelName[i], true))
        {
            std::cout << "caractere interdit, channel entree " << channelName.size() << std::endl;
            std::cout << "JOIN 5.3" << std::endl;
            server.sendMessage("479 " + client.getNickname() + " " + word[0] + " :Bad Channel Mask\r\n", client);
        }
        else
        {
            std::cout << "JOIN 5.4" << std::endl;
            channelName[i].erase(0, 1);
            std::cout << "JOIN 5.5" << std::endl;
            if (server.getChannels().empty() || !searchChannelMatch(server, channelName[i]))
            {
                std::cout << "JOIN 6" << std::endl;
                Server::Channel *channel = new(Server::Channel);
                
                channel->setOperator(client);
                channel->setName(channelName[i]);
                client.getChannels().push_back(channel);
                client.getOpMap()[channelName[i]] = true;
                channel->getMembers().push_back(&client);
                server.getChannels().push_back(channel);
                welcomeMessage(server, *channel, client);
                sendMessageAllClientJoin(client, server, *channel, host);
            }
            else
            {
                std::cout << "JOIN 7" << std::endl;
                try
                {
                    Server::Channel &channel = ChannelMatch(server, channelName[i]);

                    if (searchMembers(client.getNickname(), channel))
                        continue;
                    std::cout << "JOIN 7.2" << std::endl;
                    if (channel.getK())
                    {
                        std::cout << "JOIN 7.3" << std::endl;
                        if (word.size() == 2 &&  i < passwordCommand.size() && channel.getPass() == passwordCommand[i])
                        {
                            std::cout << "JOIN 7.4" << std::endl;
                            if (channel.getL() && channel.getMembers().size() + 1 > channel.getMembersLimit())
                            {
                                std::cout << "JOIN 7.5" << std::endl;
                                server.sendMessage("475 " + client.getNickname() + " " + tmp + " :Limit reached, you cannot join channel (+l)\r\n", client);
                                continue;
                            }
                            else if (channel.getI() && !channel.hasBeenInvited(client.getNickname()))
                            {
                                server.sendMessage("475 " + client.getNickname() + " " + tmp + " :You cannot join channel without invitation (+i)\r\n", client);
                                continue;
                            }
                            std::cout << "JOIN 7.6" << std::endl;
                            server.addClientInChannel(channel, client);
                            sendMessageAllClientJoin(client, server, channel, host);
                        }
                        else
                        {
                            std::cout << "JOIN 7.65" << std::endl;
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
                            std::cout << "JOIN 7.7" << std::endl;
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
                        std::cout << "JOIN 7.8" << std::endl;
                        server.addClientInChannel(channel, client);
                        std::cout << "JOIN 7.9" << std::endl;
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
            std::cout << "Name channel : " << server.getChannels()[i]->getName() << std::endl;
        }
}
