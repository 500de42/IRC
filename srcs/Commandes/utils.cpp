#include "../../includes/Serveur.hpp"
#include "../../includes/Client.hpp"

void processCommand(Client &client, Server &server, int bytes, size_t *i)
{
    std::string &line = client.getRealBuffer();
    std::string extract;
    int pos;
    
    while (line.find("\r\n") != std::string::npos)
    {
        pos = line.find("\r\n");
        extract = line.substr(0, pos);
        execCommand((char *)extract.c_str(), client, server, i);
        line.erase(0, pos + 2);
    }
}

void execCommand(char *buff , Client &tmp, Server &server, size_t *i)
{
    std::cout << "\n\ntest8\n\n";
    std::cout << "buffer: "<< buff << "\n\ntest9\n\n";
    if (!tmp.getGivenPassword())
    {
        std::cout << "\n\ntest 10 BUFFER: " << buff << "\n\n";
        std::string str(buff);
        if (!strncmp(buff, "PASS ", 5))
        {
            std::cout << "\n\ntest extract pass: " << extractPass(str) << " PASS DU SERVER : " << server.getPass() << "\n\n";
            if (extractPass(str) == server.getPass())
                tmp.onPass();
            else
            {
                std::cout << "\n\ntest 11 \n\n";
                server.sendMessage("464 * :Password incorrect\r\n", tmp);
                close(tmp.getSocket());
                server.getClients().erase(server.getClients().begin() + ((*i) - 1));
                server.getFds().erase(server.getFds().begin() + (*i));
                delete &tmp;
                (*i)--;
                return;
            }
        }
        else 
        {
            server.sendMessage("464 * :Password incorrect\r\n", tmp);
            close(tmp.getSocket());
            server.getClients().erase(server.getClients().begin() + *i);
            server.getFds().erase(server.getFds().begin() + (*i - 1));
            delete &tmp;
            i--;
            std::cout << "Rejet de connexion. FD: " << tmp.getSocket() << " Première commande invalide: " << buff << "\n";
            return;
        }
    }
    else if (!tmp.getRegister())
    {
        std::cout << "usernick entree\n";
        if (!strncmp(buff, "USER ", 5) || !strncmp(buff, "NICK ", 5))
        {
            setUserAndNick(tmp, server, buff);
            std::cout << "setusernick fini11\n";
        }
        else
        {
            std::cout << "Not registered client : " << buff  << "\n";
            server.sendMessage("451 " + (std::string)buff + " :You may not registered\r\n", tmp);
        }
        std::cout << "setusernick fini\n";
    }
    else
    { // METTRE EN PLACE POINTEUR SUR METHODES
        if (!strncmp(buff, "JOIN ", 5))
            JOIN(tmp, server, buff + 4);
        else if (!strncmp(buff, "KICK ", 5))
        {
        }
        else if (!strncmp(buff, "INVITE ", 7))
        {
        }
        else if (!strncmp(buff, "TOPIC ", 6))
        {
        }
        else if (!strncmp(buff, "MODE ", 5))
        {
            MODE(server, tmp, buff);
        }
        else if (!strncmp(buff, "USER ", 5))
        {
            server.sendMessage("462" + (std::string)buff + ":You may not reregister\r\n", tmp);
        }
        else if (!strncmp(buff, "NICK ", 5))
        {
        }
        else
        {
        }
    }
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

    if (line.size() > 2 && tmp[0] != ' ')
        return "";
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
                if (client.getChannels()[i]->getName() == channel.getName())
                {
                    client.getChannels().erase(client.getChannels().begin() + it);
                    
                }
                // if (client.getOpMap()) set pour enlever l op map
            }
            return ;
        }
    }
    return ;
}

void sendMessageAllClientKick(Server &server, std::vector<std::string> words)
{   
    for(size_t i = 0; i < server.getClients().size(); i++)
    {
        server.sendMessage(words[1] + ": KICK: #" + words[0] + " " + words[2], *server.getClients()[i]);
    }
}
