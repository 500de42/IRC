#include "../../includes/Serveur.hpp"
#include "../../includes/Client.hpp"

// 473 <nick> #private :Cannot join channel (+i)\r\n channel prive //

void extractAndSetJoin(Client &client, Server &server, std::string tmp)
{
    std::stringstream ss(tmp);
    std::vector<std::string> word;
    std::string w;
    int nbPassword = -1;
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
    std::vector<std::string> channelCommand = removeCharacter(splitCommand(word[0], ','), '#');
    for (int i = 0; i < channelCommand.size(); i++)
    {
        std::cout << "Name channel : " << channelCommand[i] << std::endl;
    }
    std::cout << "JOIN 2.6" << std::endl;
    std::vector<std::string> passwordCommand;
    std::cout << "JOIN 3" << std::endl;
    if(word.size() == 2)
    {
        passwordCommand = splitCommand(word[1], ',');
        std::cout << "JOIN 4" << std::endl;
        if (passwordCommand.size() > channelCommand.size())
        {
            server.sendMessage("461 " + client.getNickname() + " JOIN :Invalid channel/key list\r\n", client);
            return ;
        }
        else if (channelCommand.size() > passwordCommand.size())
            nbPassword = passwordCommand.size();
    }
    std::cout << "JOIN 5" << std::endl;
    for (size_t i = 0; i < channelCommand.size(); i++)
    {std::cout << "JOIN 5.2" << std::endl;
        if(prohibidedCharacterJoin(channelCommand[i], true))
        {            std::cout << "channel entree " << channelCommand.size() << std::endl;

            std::cout << "JOIN 5.3" << std::endl;
            server.sendMessage("476 " + client.getNickname() + " " + channelCommand[i] + " :Bad Channel Mask\r\n", client);
        }
        else
        {std::cout << "JOIN 5.4" << std::endl;
            if (server.getChannels().empty() || !searchChannelMatch(server, channelCommand[i]))
            {
                std::cout << "JOIN 6" << std::endl;
                Server::Channel *channel = new(Server::Channel);
                channel->getMembers().push_back(&client);
                channel->setOperator(client);
                channel->setName(channelCommand[i]);
                client.getChannels().push_back(channel);
                server.getChannels().push_back(channel);
            }
            else
            {
                try
                {
                    Server::Channel &channel = ChannelMatch(server, channelCommand[i]);
                    if (channel.getK())
                    {
                        if (word.size() == 2 && channel.getPass() == passwordCommand[i])
                        {
                            if (channel.getL() && channel.getMembers().size() + 1 > channel.getMembersLimit())
                            {
                                server.sendMessage("475 " + client.getNickname() + " " + tmp + " :Cannot join channel (+l)\r\n", client);
                                continue;
                            }
                            client.getChannels().push_back(&channel);
                            channel.getMembers().push_back(&client);
                        }
                        else
                            server.sendMessage("475 " + client.getNickname() + " " + passwordCommand[i] + " :Cannot join channel (+k)\r\n", client);
                    }
                    else if (channel.getL() && channel.getMembers().size() + 1 > channel.getMembersLimit())
                    {
                        server.sendMessage("475 " + client.getNickname() + " " + tmp + " :Cannot join channel (+l)\r\n", client);
                        continue;
                    }
                    else
                    {
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
