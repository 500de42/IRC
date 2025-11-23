#include "../../includes/Serveur.hpp"
#include "../../includes/Client.hpp"


int checkArg(int ac, char **av)
{
	if (ac != 3)
	{
		std::cout << "the program must be run like this < ./ircserv <port> <password> >\n";
		return (1);
	}
	if (prohibitedCharacterServerPassword((std::string)av[2]))
	{
		std::cout << "The password contains prohibited characters.\n";
		return (1);
	}
	if (!checkNum((std::string)av[1]) || std::atoi(av[1]) < 1024 || std::atoi(av[1]) > 65535)
	{
		std::cout << "The port is not correct\n";
		return (1);
	}
    return 0;
}


void processCommand(Client &client, Server &server, size_t *i)
{
    std::string &line = client.getRealBuffer();
    std::string extract;
    int pos;
    
    while (line.find("\r\n") != std::string::npos)
    {
        pos = line.find("\r\n");
        extract = line.substr(0, pos);
        if (execCommand((char *)extract.c_str(), client, server, i))
            line.erase(0, pos + 2);
    }
}

bool execCommand(char *buff , Client &tmp, Server &server, size_t *i)
{(void)i;

    if (!tmp.getRegister())
    {
        if (!strncmp(buff, "USER ", 5) || !strncmp(buff, "NICK ", 5))
        {
            setUserAndNick(tmp, server, buff);
        }
        else if (!strncmp(buff, "PASS ", 5) && !tmp.getGivenPassword())
        {
            std::string str(buff);
            if (extractPass(str) == server.getPass())
                tmp.onPass();
            else
                server.sendMessage("464 * :Password incorrect\r\n", tmp);
        }
        else
        {
            server.sendMessage("451 " + (std::string)buff + " :You may not registered\r\n", tmp);
            return true;
        }
        if (!tmp.getNickname().empty() && !tmp.getUsername().empty() && tmp.getGivenPassword())
        {
            tmp.onRegisted();
            welcomeMessage(server, tmp);
        }
    }
    else
    { // METTRE EN PLACE POINTEUR SUR METHODES
        if (!strncmp(buff, "JOIN ", 5))
            JOIN(tmp, server, buff + 4);
        else if (!strncmp(buff, "KICK ", 5))
            KICK(server, tmp, buff);
        else if (!strncmp(buff, "INVITE ", 7))
            INVITE(tmp, server, buff);
        else if (!strncmp(buff, "TOPIC ", 6))
            TOPIC(server, tmp, buff + 5);
        else if (!strncmp(buff, "MODE ", 5))
            MODE(server, tmp, buff);
        else if (!strncmp(buff, "PRIVMSG ", 8))
            PRIVMSG(server, tmp, buff + 7);
        else if (!strncmp(buff, "USER ", 5))
            server.sendMessage("462" + (std::string)buff + ":Unauthorized command (already registered)\r\n", tmp);
        else if (!strncmp(buff, "NICK ", 5))
            NICK(server, tmp, buff);
        else if (!strncmp(buff, "PING ", 5))
            PING(server, tmp, buff);
        else
        {
            server.sendMessage("421 " + tmp.getNickname() + " \"" + (std::string)buff + "\" :Unknown command\r\n", tmp);
        }
    }
    return true;
}

void PING(Server &server, Client &client, char *buff)
{
    std::string buffer(buff);
    std::stringstream ss(buffer);
    std::vector<std::string> words;
    std::string w;

    while (ss >> w)
        words.push_back(w);
    if(words.size() != 2)
    {
        server.sendMessage("461 " + client.getNickname() + " PING :Not enough parameters\r\n", client);
        return ;
    }
    server.sendMessage("PONG : " + words[1] + "\r\n", client);
}


std::string joinVector(const std::vector<std::string> &vec, char sep)
{
    std::string result;
    for (size_t i = 0; i < vec.size(); ++i)
    {
        if (i > 0)         
            result += sep;  
        result += vec[i];
    }
    return result;
}

bool prohibidedCharacter(std::string tmp)
{
    for (std::string::iterator i = tmp.begin(); i != tmp.end(); i++)
    {
        if (*i == ' ' || *i == ',' || *i == '*' || *i == '?' 
            || *i == '!' || *i == '@')
        {
            return true;
        }
    }
    return false;
}

std::string extractMessage(char *tmp)
{
    if (!tmp)
        return "";
    std::string line(tmp);

    if (line.size() < 2 && tmp[0] != ' ')
        return "";
    line.erase(0, 1);
    return line;
}

bool searchChannelMatch(Server server, std::string name)
{
    for(size_t i = 0; i < server.getChannels().size(); i++)
    {
        if (name == server.getChannels()[i]->getName())
            return true;
    }
    return false;
}

Server::Channel &ChannelMatch(Server server, std::string name)
{
    for(size_t i = 0; i < server.getChannels().size(); i++)
    {
        if (name == server.getChannels()[i]->getName())
            return *server.getChannels()[i];
    }
    throw std::runtime_error("Channel not found");
}

Client &clientMatch(std::string name, Server server)
{
    for(size_t i = 0; i < server.getClients().size(); i++)
    {
        if (name == server.getClients()[i]->getNickname())
            return *server.getClients()[i];
    }
    throw std::runtime_error("Client not found");
}

bool prohibidedCharacterJoin(std::string tmp, bool checkFirst)
{
    if (checkFirst && tmp[0] != '#' && tmp[0] != '&')
        return true;
    for (std::string::iterator i = tmp.begin(); i != tmp.end(); i++)
    {
        if (*i == ' ' || *i == ',' || *i == ':')
            return true;
    }
    return false;
}

std::vector<std::string> splitCommand(std::string command, char c)
{
    std::vector<std::string> word;
    int start = 0;
    int end = 0;
    if (command.find(c) == std::string::npos)
    {
        word.push_back(command);
        return word;
    }
    while(command[end])
    {
        while(command[end] == c && command[end])
            end++; 
        start = end;               
        while (command[end] != c && command[end])
            end++;
        std::string tmp = command.substr(start, end - start);
        word.push_back(tmp);
    }

    return word;
}

std::string extractPass(std::string pass)
{
    if (pass.empty())
        return "";
    std::vector<std::string> vec;
    std::stringstream ss(pass);
    std::string line;
    while (ss >> line)
        vec.push_back(line);
    if (vec.size() != 2)
        return "";
    return vec[1];
}

bool prohibitedCharacterServerPassword(std::string word)
{
    for (std::string::iterator i = word.begin(); i != word.end(); i++)
    {
        if (*i == ',' || *i == '\n' || *i == '\r')
            return true;
    }
    return false;
}

bool searchMembers(std::string name, Server::Channel channel)
{
    for(size_t i = 0; i < channel.getMembers().size(); i++)
    {
        if (channel.getMembers()[i]->getNickname() == name)
            return true;  
    }
    return false;
}

void removeChannelMember(Server::Channel &channel, Client &client)
{
    for(size_t i = 0; i < channel.getMembers().size(); i++)
    {
        if (channel.getMembers()[i]->getNickname() == client.getNickname())
        {
            channel.getMembers().erase(channel.getMembers().begin() + i);
            for(size_t it = 0; it < client.getChannels().size(); it++)
            {
                if(client.getChannels().size())
                {
                    if (client.getChannels()[it]->getName() == channel.getName())
                    {    
                        client.getChannels().erase(client.getChannels().begin() + it);
                        break;
                    }
                }
                else
                {    
                    break;
                }
            }
            return ;
        }
    }
    return ;
}

void sendMessageAllClientKick(Server &server, Server::Channel &channel, std::vector<std::string> words, Client client)
{   
    std::string tmp(client.getNickname() + "!" + client.getUsername() +"@127.0.0.1 : KICK: #");

    for(size_t i = 0; i < channel.getMembers().size(); i++)
    {
        if(words.size() == 3)
            server.sendMessage(tmp + words[0] + " " + words[1] + " :" + words[2] + "\r\n", *channel.getMembers()[i]);
        else if (words.size() == 2)
            server.sendMessage(tmp + words[0] + " " + words[1] + "\r\n", *channel.getMembers()[i]);
    }
}

void sendMessageAllClient(Server &server, Server::Channel &channel, std::string message)
{
    for(size_t i = 0; i < channel.getMembers().size(); i++)
    {
        server.sendMessage(message + "\r\n", *channel.getMembers()[i]);
    }
}

void sendMessageAllClientJoin(Client &client, Server &server, Server::Channel &channel, std::string message)
{
    for(size_t i = 0; i < channel.getMembers().size(); i++)
    {
        server.sendMessage(message + "\r\n", *channel.getMembers()[i]);
    }
    if (!channel.getTopic().empty())
        server.sendMessage("332 " + client.getNickname() + " #" + channel.getName() + " " + channel.getTopic() + "\r\n", client);
}
        
void welcomeMessage(Server &server, Client  &client)
{   
    server.sendMessage(":IRCserver 001 " + client.getNickname() + " :Welcome to the Internet Relay Network " + client.getNickname() + "!" + client.getUsername() +"@127.0.0.1\r\n", client);
    server.sendMessage(":IRCserver 002 " + client.getNickname() + " :Your host is IRCSERVEUR, running version 1.0\r\n", client);
    server.sendMessage(":IRCserver 003 " + client.getNickname() + " :This server was created " + server.actualTime() + "\r\n", client);
    server.sendMessage(":IRCserver 004 " + client.getNickname() + " :" +client.getNickname() + " IRCserver 1.0 o itklo" +  "\r\n", client);
}

std::vector<std::string> removeCharacter(std::vector<std::string> vec, char c)
{
    std::vector<std::string> word;
    std::string w;
    int check = 0;
    for (std::vector<std::string>::iterator it = vec.begin(); it != vec.end(); it++)
    {    
        for (size_t i = 0; i < (*it).size(); i++)
        {
            if ((*it)[i] == c)
            {
                check++;
                (*it).erase(i, 1);
                word.push_back((*it));
            }
        }
    }
    if (!check)
        return vec;
    return word;
}

std::string timeToString(time_t val) 
{
    std::stringstream ss;
    ss << val;
    return ss.str();
}

void welcomeChannelMessage(Server &server, Client  &client, Server::Channel &channel)
{
    time_t tt = time(NULL);
    if (!channel.getTopic().empty())
    {
        server.sendMessage(":IRCserver 332 " + client.getNickname() + " " + channel.getName() + " :" + channel.getTopic() + "\r\n", client);
        server.sendMessage(":IRCSERVEUR 333 " + client.getNickname() + channel.getName() + " " + timeToString(channel.getLastTopicTime()) + "\r\n", client);    
    }
    else
    {
        server.sendMessage(":IRCSERVEUR 331 " + client.getNickname() + channel.getName() + " " + timeToString(tt) + "\r\n", client);
    }
    std::string clientList = ":";
    for(std::vector<Client *>::iterator it = channel.getMembers().begin(); it != channel.getMembers().end(); it++)
    {
        if ((*it)->getOp(channel.getName()))
            clientList += "@" + (*it)->getNickname() + " ";
        else
            clientList += (*it)->getNickname() + " ";
    }
    clientList.erase(clientList.size() - 1);
    server.sendMessage(":IRCSERVEUR 353 " + client.getNickname() + " = #" + channel.getName() + " " + clientList +  "\r\n", client);
    server.sendMessage(":IRCSERVEUR 366 " + client.getNickname() + " #" + channel.getName() + " :End of /NAMES list.\r\n", client);    
}
