#include "../includes/Serveur.hpp"
#include "../includes/Client.hpp"

void JOIN(Client &client, Server &server, const char *tmp)
{
    std::string buffer(tmp);

    if (buffer.size() < 2)
    {
        server.sendMessage("461 " + client.getNickname() + " JOIN :Not enough parameters\r\n", client);
        return ;
    }
    extractAndSetJoin(client, server, buffer);// envoyer a l espace apres join //
}

// 473 <nick> #private :Cannot join channel (+i)\r\n channel prive //

void extractAndSetJoin(Client &client, Server &server, std::string tmp)
{
    std::stringstream ss(tmp);
    std::vector<std::string> word;
    std::string w;
    int nbPassword = -1;
    
    while (ss >> w)
        word.push_back(w);
    if (word.size() > 2 || word.empty())
    {
        server.sendMessage("461 " + client.getNickname() + " JOIN :Too many parameters\r\n", client);
        return ;
    }
    if (word.size() == 1 && word[0].size() == 1 && word[0][0] == '0')
    {
        // gerer tout les channel a quitter //
    }
    std::vector<std::string> channelCommand = splitCommand(word[0], ',');
    std::vector<std::string> passwordCommand;

    if(word.size() == 2)
    {
        passwordCommand = splitCommand(word[1], ',');
        if (passwordCommand.size() > channelCommand.size())
        {
            server.sendMessage("461 " + client.getNickname() + " JOIN :Invalid channel/key list\r\n", client);
            return ;
        }
        else if (channelCommand.size() > passwordCommand.size())
            nbPassword = passwordCommand.size();
    }
    for (int i = 0; i < channelCommand.size(); i++)
    {
        if(prohibidedCharacterJoin(channelCommand[i], true))
            server.sendMessage("476 " + client.getNickname() + " " + word[i] + " :Bad Channel Mask\r\n", client);
        else
        {
            if (server.getChannels().empty() || !searchChannelMatch(server, word[i]))
            {
                Server::Channel channel;
                channel.getMembers().push_back(&client);
                channel.setFirst(client);
                channel.setName(word[i]);
                if(word.size() == 2)
                    channel.setPasssword(passwordCommand[i]);
                client.getChannels().push_back(&channel);
                server.getChannels().push_back(&channel);
            }
            else
            {
                try{
                    Server::Channel &channel = ChannelMatch(server, word[i]);
                    if (channel.getPasswordMode())
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
                        channel.getMembers().push_back(&client);
                        client.getChannels().push_back(&channel);
                    }
                }
                catch(std::exception &e) {}
            }
        }
    }
}

bool searchChannelMatch(Server server, std::string name)
{
    for(int i = 0; i < server.getChannels().size(); i++)
    {
        if (name == server.getChannels()[i]->getName())
            return true;
    }
    return false;
}

Server::Channel &ChannelMatch(Server server, std::string name)
{
    for(int i = 0; i < server.getChannels().size(); i++)
    {
        if (name == server.getChannels()[i]->getName())
            return *server.getChannels()[i];
    }
    throw std::runtime_error("Channel not found");
}

bool prohibidedCharacterJoin(std::string tmp, bool checkFirst)
{
    if (checkFirst && tmp[0] != '#')
        return true;
    for (std::string::iterator i = tmp.begin(); i != tmp.end(); i++)
    {
        if (*i == ' ' || *i == ',' || *i == '*')
            return true;
    }
    return false;
}

std::vector<std::string> splitCommand(std::string command, char c)
{
    std::vector<std::string> word;
    std::string w;
    int first = 0;
    
    for (std::string::iterator i = command.begin(); i != command.end(); i++)
    {
        if (*i == c)
        {
            std::string::iterator other = i;
            if (first == 0)
            {
                std::string tmp(command.begin(), i);
                first = 1;
                word.push_back(tmp);
            }
            else
            {
                std::string tmp(other + 1, i);
                word.push_back(tmp);
            }
        }
    }
    if (!first)
        word.push_back(command);
    return word;
}
