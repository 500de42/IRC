#include "../../includes/Serveur.hpp"
#include "../../includes/Client.hpp"


void TOPIC(Server &server, Client &client, const char *tmp)
{
    std::stringstream ss(tmp);
    std::string buffer(tmp);
    std::vector<std::string> word;
    std::string channelName;
    std::string newTopic;
    std::string w;

    std::cout << "JOIN 1" << std::endl;
    while (ss >> w)
        word.push_back(w);

    if  (word.empty() || word[0].size() < 2 || *word[0].begin() != '#')
    {
        server.sendMessage("461 " + client.getNickname() + " TOPIC :Not enough parameters\r\n", client);
        return ;
    }
    if (word.size() >= 2)
    {
        int pos = buffer.find(':');
        if (pos)
        {
            newTopic = buffer.substr(pos);
            newTopic.erase(0, 1);
        }
        else
        {
            server.sendMessage("461 " + client.getNickname() + " TOPIC :Not enough parameters\r\n", client);
            return;
        }
    }
    try
    {
        Server::Channel &channel = ChannelMatch(server, channelName);
            
        if (!searchMembers(client.getNickname(), channel))
        {
            server.sendMessage("442 " + client.getNickname() + " TOPIC :You are not channel member\r\n", client);
            return;
        }
        if (word.size() == 1)
        {
            if (channel.getTopic().empty())
                server.sendMessage(":IRCSERVEUR 331 " + client.getNickname() + " " + channelName + ": No topic is set\r\n", client);
            else
            {
                server.sendMessage(":IRCSERVEUR 332 " + client.getNickname() + " " + channelName + ": " + channel.getTopic() + "\r\n", client);
            }
        }
        else
        {
            if (channel.getT() && !client.getOp(channel.getName()))
            {
                server.sendMessage("482 " + client.getNickname() + " TOPIC :You not channel operator\r\n", client);
                return;
            }
            if (newTopic.empty())
                channel.getTopic() = "";
            else
                channel.getTopic() = newTopic;
            std::string t(client.getNickname() + "!" + client.getUsername() +"@127.0.0.1 : TOPIC: #" + channel.getName() + ":");
            sendMessageAllClient(server, channel, t + newTopic);
            if (!channel.getLastTopicSetter().empty())
                server.sendMessage(":IRCSERVEUR 333 " + client.getNickname() + channel.getName() + currentTime + "\r\n", client);
            channel.getLastTopicSetter() = client.getNickname();
        }
    }
    catch(std::exception &e)
    {
        server.sendMessage("403 " + client.getNickname() + " TOPIC :Channel("+ channelName +") don't exist\r\n", client);
    }
}