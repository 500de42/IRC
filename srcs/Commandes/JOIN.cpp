#include "../../includes/Serveur.hpp"
#include "../../includes/Client.hpp"

// 473 <nick> #private :Cannot join channel (+i)\r\n channel prive //

void extractAndSetJoin(Client &client, Server &server, std::string tmp)
{
    std::stringstream ss(tmp);
    std::vector<std::string> word;
    std::string w;
    bool setPass = true;

    std::cout << "JOIN 1" << std::endl;
    while (ss >> w)
        word.push_back(w);
    if (word.size() > 2 || word.empty())
    {
        server.sendMessage("461 " + client.getNickname() + " JOIN :Too many parameters\r\n", client);
        return ;
    }
    std::cout << "JOIN 2" << std::endl;
    if (word.size() == 1 && word[0].size() == 1 && word[0][0] == '0')
    {
        // gerer tout les channel a quitter //
    }
    std::cout << "JOIN 2.5" << std::endl;
    std::vector<std::string> channelCommand = splitCommand(word[0], ',');
    std::cout << "JOIN 2.6" << std::endl;
    std::vector<std::string> passwordCommand;
    std::cout << "JOIN 3" << std::endl;
    if(word.size() == 2)
    {
        std::cout << "JOIN 3.5" << std::endl;
        passwordCommand = splitCommand(word[1], ',');
        std::cout << "JOIN 4" << std::endl;
        if (passwordCommand.size() > channelCommand.size())
        {
            server.sendMessage("461 " + client.getNickname() + " JOIN :Invalid channel/key list\r\n", client);
            return ;
        }
    }
    std::cout << "JOIN 5" << std::endl;
    for (size_t i = 0; i < channelCommand.size(); i++)
    {  
        std::cout << "JOIN 5.2" << std::endl;
        if(prohibidedCharacterJoin(channelCommand[i], true))
        {
            std::cout << "caractere interdit, channel entree " << channelCommand.size() << std::endl;
            std::cout << "JOIN 5.3" << std::endl;
            server.sendMessage("476 |" + client.getNickname() + "| " + word[0] + " :Bad Channel Mask\r\n", client);
        }
        else
        {std::cout << "JOIN 5.4" << std::endl;
            channelCommand[i].erase(0, 1);
        std::cout << "JOIN 5.5" << std::endl;
            if (server.getChannels().empty() || !searchChannelMatch(server, channelCommand[i]))
            {
                std::cout << "JOIN 6" << std::endl;
                Server::Channel *channel = new(Server::Channel);
                channel->getMembers().push_back(&client);
                channel->setOperator(client);
                channel->setName(channelCommand[i]);
                client.getChannels().push_back(channel);
                client.getOpMap()[channelCommand[i]] = true;
                server.getChannels().push_back(channel);
            }
            else
            {
                std::cout << "JOIN 7" << std::endl;
                try
                {
                    Server::Channel &channel = ChannelMatch(server, channelCommand[i]);
                    std::cout << "JOIN 7.2" << std::endl;
                    if (channel.getK())
                    {std::cout << "JOIN 7.3" << std::endl;
                        if (word.size() == 2 && channel.getPass() == passwordCommand[i])
                        {std::cout << "JOIN 7.4" << std::endl;
                            if (channel.getL() && channel.getMembers().size() + 1 > channel.getMembersLimit())
                            {std::cout << "JOIN 7.5" << std::endl;
                                server.sendMessage("475 " + client.getNickname() + " " + tmp + " :Cannot join channel (+l)\r\n", client);
                                continue;
                            }std::cout << "JOIN 7.6" << std::endl;
                            client.getChannels().push_back(&channel);
                            channel.getMembers().push_back(&client);
                        }
                        else
                            server.sendMessage("475 " + client.getNickname() + " " + passwordCommand[i] + " :Cannot join channel (+k)\r\n", client);
                    }
                    else if (channel.getL() && channel.getMembers().size() + 1 > channel.getMembersLimit())
                    {std::cout << "JOIN 7.7" << std::endl;
                        server.sendMessage("475 " + client.getNickname() + " " + tmp + " :Cannot join channel (+l)\r\n", client);
                        continue;
                    }
                    else
                    {std::cout << "JOIN 7.8" << std::endl;
                        std::cout << "channel ajouter" << std::endl;
                        channel.getMembers().push_back(&client);
                        client.getChannels().push_back(&channel);
                        std::cout << "channel ajouter2" << std::endl;
                    }
                }
                catch(std::exception &e) {
                    std::cout << "le channel ne s est pas ajouter, nom du channel: " << word[0] << std::endl;
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
        for (int i = 0; i < server.getChannels().size(); i++)
        {
            std::cout << "Name channel : " << server.getChannels()[i]->getName() << std::endl;
        }
}
