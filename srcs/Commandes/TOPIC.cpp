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

    while (ss >> w)
        word.push_back(w);

    if  (word.empty() || word[0].size() < 2 || (*word[0].begin() != '#' && *word[0].begin() != '&'))
    {
        server.sendMessage("461 " + client.getNickname() + " TOPIC :Not enough parameters\r\n", client);
        return ;
    }
    if (word.size() >= 2)
    {
        int pos = buffer.find(':');
        if (pos != (int)std::string::npos)
        {
            newTopic = buffer.substr(pos);
            newTopic.erase(0, 1);
            std::cout << "new topic " << newTopic << std::endl;
        }
        else
        {
            server.sendMessage("461 " + client.getNickname() + " TOPIC :Not enough parameters\r\n", client);
            return;
        }
    }
    word[0].erase(0,1);
    channelName = word[0];
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
                std::cout << "topic " << channel.getTopic() << std::endl;
                server.sendMessage(":IRCSERVEUR 332 " + client.getNickname() + " " + channelName + " :" + channel.getTopic() + "\r\n", client);
                server.sendMessage(":IRCSERVEUR 333 " + client.getNickname() + " " + channel.getName() + " " + channel.getLastTopicSetter() + " " + timeToString(channel.getLastTopicTime()) + "\r\n", client);
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
            std::cout << "topic " << channel.getTopic() << std::endl;
            std::string t(client.getNickname() + "!" + client.getUsername() +"@127.0.0.1 : TOPIC: #" + channel.getName() + ":");
            sendMessageAllClient(server, channel, t + newTopic);
            channel.setLastTopicTime(time(NULL));
            if (!channel.getLastTopicSetter().empty())
                server.sendMessage(":IRCSERVEUR 333 " + client.getNickname() + channel.getName() + " " + timeToString(channel.getLastTopicTime()) + "\r\n", client);
            channel.getLastTopicSetter() = client.getNickname();
        }
    }
    catch(std::exception &e)
    {
        server.sendMessage("403 " + client.getNickname() + " TOPIC :Channel("+ channelName +") don't exist\r\n", client);
    }
}