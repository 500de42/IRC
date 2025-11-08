#include "../../includes/Serveur.hpp"
#include "../../includes/Client.hpp"

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
    std::cout << "\n\ntest8\n\n";
    std::cout << "buffer: "<< buff << "\n\ntest9\n\n";

    if (!tmp.getRegister())
    {
        std::cout << "usernick entree\n";
        if (!strncmp(buff, "USER ", 5) || !strncmp(buff, "NICK ", 5))
        {
            setUserAndNick(tmp, server, buff);
            std::cout << "setusernick fini11\n";
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
            std::cout << "Not registered client : " << buff  << "\n";
            server.sendMessage("451 " + (std::string)buff + " :You may not registered\r\n", tmp);
            return true;
        }
        if (!tmp.getNickname().empty() && !tmp.getUsername().empty() && tmp.getGivenPassword())
        {
            std::cout << "Nouvelle connexion acceptée. FD: "  << tmp.getSocket() << std::endl;
            tmp.onRegisted();
            server.sendMessage("001 " + tmp.getNickname() +  " :Welcome to the Internet Relay Network " + tmp.getNickname() + "!" + tmp.getUsername() + "@127.0.0.1", tmp);
        }
        std::cout << "setusernick fini\n";
    }
    else
    { // METTRE EN PLACE POINTEUR SUR METHODES
        if (!strncmp(buff, "JOIN ", 5))
            JOIN(tmp, server, buff + 4);
        else if (!strncmp(buff, "KICK ", 5))
            KICK(server, tmp, buff);
        else if (!strncmp(buff, "INVITE ", 7))
        {
        }
        else if (!strncmp(buff, "TOPIC ", 6))
        {
        }
        else if (!strncmp(buff, "MODE ", 5))
            MODE(server, tmp, buff);
        else if (!strncmp(buff, "USER ", 5))
            server.sendMessage("462" + (std::string)buff + ":You may not reregister\r\n", tmp);
        else if (!strncmp(buff, "NICK ", 5))
        {
        }
        else
        {
        }
    }
    return true;
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
        if ((*i == ' ' || *i == ',' || *i == '*' || *i == '?' 
            || *i == '!' || *i == '@' || *i >= 0) && *i <= 31)
        {
            std::cout << ":server_name 461 * USER :Not enough parameters\n";
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

    std::cout << "|" << line << "|" << std::endl;
    if (line.size() < 2 && tmp[0] != ' ')
        return "";
    line.erase(0, 1);
    std::cout << "|" << line << "|" << std::endl;
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
                {    if (client.getChannels()[i]->getName() == channel.getName())
                        client.getChannels().erase(client.getChannels().begin() + it);}
                else
                    std::cout << "channel client vide \n";
            }
            return ;
        }
    }
    return ;
}

void sendMessageAllClientKick(Server &server, Server::Channel &channel, std::vector<std::string> words)
{   
    for(size_t i = 0; i < channel.getMembers().size(); i++)
    {
        server.sendMessage(words[1] + ": KICK: #" + words[0] + " " + words[2] + "\r\n", *channel.getMembers()[i]);
    }
}

void welcomeMessage(Server &server, Server::Channel &channel, Client  &client)
{   
    server.sendMessage(":IRCserver 001 " + client.getNickname() + " :Welcome to the Internet Relay Network " + client.getNickname() + "!" + client.getUsername() +"@127.0.0.1\r\n", client);
    server.sendMessage(":IRCserver 002 " + client.getNickname() + " :Your host is " + channel.getName() + ", running version 1.0\r\n", client);
    server.sendMessage(":IRCserver 003 " + client.getNickname() + " :This server was created<date a init>\r\n", client);
    server.sendMessage(":IRCserver 004 " + client.getNickname() + " :" +client.getNickname() + " IRCserver 1.0 o itklo" +  "\r\n", client);
    // server.sendMessage(":IRCserver 321 " + client.getNickname() + " :Channel :Users Topic\r\n",client);
    // server.sendMessage(":IRCserver 323 " + client.getNickname() + " :End of /LIST\r\n",client);
}

std::vector<std::string> removeCharacter(std::vector<std::string> vec, char c)
{
    std::vector<std::string> word;
    std::string w;
    int check = 0;
    std::cout << vec[0] << " " << vec[1] << " " << vec[2] << std::endl;
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

    // for(size_t i = 0; i < channel.getMembers().size(); i++)
    // {
    //     server.sendMessage(":" + client.getNickname() + "!" + client.getUsername() +"@127.0.0.1 JOIN #" + channel.getName(), *channel.getMembers()[i]);
    // }
    // if (channel.getT())
    //     server.sendMessage(": 332 " + client.getNickname() + " " + channel.getName() + ":" + channel.getTopic(), client);
    // server.sendMessage(": 353 " + client.getNickname() + " = " + channel.getName() + ":", client);   
    // for(size_t i = 0; i < channel.getMembers().size(); i++)
    // {
    //     server.sendMessage(": 353 " + client.getNickname() + " = " + channel.getName() + ":" + channel.getTopic(), client);
    // }


