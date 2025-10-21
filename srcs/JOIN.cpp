#include "../includes/Serveur.hpp"
#include "../includes/Client.hpp"

bool searchChannelMatch(Server server, std::string name)
{
    for(int i = 0; i < server.getChannels().size(); i++)
    {
        if (name == server.getChannels()[i].getName())
            return true;
    }
    return false;
}

Server::Channel &ChannelMatch(Server server, std::string name)
{
    for(int i = 0; i < server.getChannels().size(); i++)
    {
        if (name == server.getChannels()[i].getName())
            return server.getChannels()[i];
    }
    throw std::runtime_error("Channel not found");
}

bool prohibidedCharacterJoin(std::string tmp)
{
    if (tmp[0] != '#')
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
    return word;
}

bool extractAndSetJoin(Client &client, Server &server, std::string tmp)
{
    std::stringstream ss(tmp);
    std::vector<std::string> word;
    std::string w;
    int nbPassword = 0;
    
    while (ss >> w)
        word.push_back(w);
    int v = word[1].find_first_of(',', 0);
    if (v)
    {
        std::vector<std::string> channelCommand = splitCommand(word[1], ',');
        std::vector<std::string> passwordCommand = splitCommand(word[2], ',');
        if (passwordCommand.size() > channelCommand.size())
        {
            server.sendMessage("461 " + client.getNickname() + " JOIN :Not enough parameters\r\n", client);
            return false;
        }
        else if (channelCommand.size() > passwordCommand.size())
            nbPassword = passwordCommand.size();
        for (int i = 0; i < channelCommand.size(); i++)
        {
            if(prohibidedCharacterJoin(word[i]))
                server.sendMessage("476 " + client.getNickname() + " " + word[i] + " :Bad Channel Mask\r\n", client);
            else
            {
                if (server.getChannels().empty() || !searchChannelMatch(server, word[i]))
                {
                    Server::Channel channel;
                    channel.getMembers().push_back(client);
                    channel.setFirst(client);
                    channel.setName(word[i]);
                    client.getChannels().push_back(channel);
                    server.getChannels().push_back(channel);
                }
                else
                {
                    try{
                        Server::Channel &channel = ChannelMatch(server, word[i]);
                        channel.getMembers().push_back(client);
                        client.getChannels().push_back(channel);
                    }
                    catch(std::exception &e) {}
                }
            }
        }
        for (int i = 0; i < passwordCommand.size(); i++)
        {
            if (nbPassword != 0)
            {
                if(i == nbPassword)
                    return;
                try{
                    ChannelMatch(server, channelCommand[i]).setPasssword(word[i]);
                }
                catch(std::exception &e) {}
            }
            else
            {
                for(int i = 0; i != channelCommand.size(); i++)
                {   
                    try{
                        ChannelMatch(server, channelCommand[i]).setPasssword(word[i]);
                    }
                    catch(std::exception &e) {}
                }
            }
        }
    }
    else
    {

    }
}


void JOIN(Client &client, Server &server, const char *tmp)
{
    std::string buffer(tmp);

    if (buffer.size() < 2)
    {
        server.sendMessage("", client); //message a set
        return ;
    }
    extractAndSetJoin(client, server, buffer);
}
