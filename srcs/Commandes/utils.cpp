#include "../../includes/Serveur.hpp"
#include "../../includes/Client.hpp"

void processCommand(Client &client, Server &server, int bytes, size_t *i)
{
    std::string line(client.getRealBuffer());
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
                // close(tmp.getSocket());
                // server.getClients().erase(server.getClients().begin() + (i - 1));
                // server.getFds().erase(server.getFds().begin() + i);
                // delete &tmp;
                // i--;
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
            server.sendMessage("451 " + (std::string)buff + " :You have not registered\r\n", tmp);
        }
        std::cout << "setusernick fini\n";
    }
    else
    { // METTRE EN PLACE POINTEUR SUR METHODES
        if (!strncmp(buff, "JOIN ", 5))
            JOIN(tmp, server, buff);
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

bool extractAndSetMessageForUser(char *tmp, Client &client, Server &server)
{
    if (!tmp)
        return false;

    std::stringstream ss(tmp);
    std::string l;
    std::vector<std::string> tab;

    while (ss >> l)
        tab.push_back(l);
    std::cout << "test u1 size: " << tab.size() << std::endl;
    if (tab.size() < 4)
    {
        std::cout << "test u2 size: " << tab.size() << std::endl;
        server.sendMessage("461 5:Not enough parameters\r\n", client);   
        return false;
    }
    if (tab[4][0] != ':')
        return false;
    std::cout << "test u1" << std::endl;
    tab[4].erase(0, 1);
    if (tab.size() != 5)
    {
        std::vector<std::string> vec(tab.begin() + 4, tab.end());
        l = joinVector(vec, ' ');
        tab[4] = l;
    }
    if (prohibidedCharacter(tab[1]) || prohibidedCharacter(tab[4]))
    {
        std::cout << "test u2 tab 1: " << prohibidedCharacter(tab[1]) << "tab2 : " << prohibidedCharacter(tab[4]) << std::endl;
        server.sendMessage("461 22" + tab[1] + ":Not enough parameters\r\n", client);   
        return false;
    }
    // if (server.checkDoubleName(tab[1].c_str()))
    if (client.getUsername().empty())
    {
        std::cout << "test u3" << std::endl;
        client.setUsername(tab[1].c_str());
        client.setRealname(tab[4].c_str());
    }
    else
    {
        std::cout << "test u4" << std::endl;
        server.sendMessage("462" + tab[1] + ":You may not reregister\r\n", client);
        return false;
    }
    return (true);
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

// template <typename T>
// void sendMessage(T c, std::string buffer)
// {
//     T.send
// }

std::string extractMessage(char *tmp)
{
    if (!tmp)
        return "";
    std::string line(tmp);

    if (line.size() > 2 && tmp[0] != ' ')
        return "";
    return line;
}

// bool parsingSetUser(std::vector<std::string> &tab)
// {
    
// }

void setUserAndNick(Client &client, Server &server, char *buff)
{
    std::string line;
    std::vector<std::string> mess;
    std::cout << "|" << buff << "| " << strlen(buff) << std::endl;
    if (!strncmp(buff, "USER", 4))
    {
        std::cout << "test user1" << std::endl;
        if (client.getUsername().empty())          
        {
            std::cout << "test user2" << std::endl;
            if (!extractAndSetMessageForUser(buff, client, server))
            {    std::cout << "test user3" << std::endl;
                // ERR_ALREADYREGISTRED();
                // server.sendMessage("\r\n", client);//erreur a ecrire et envoyer au client
                return;
            }
        }
        else
        {
            server.sendMessage("\r\n", client);//erreur a ecrire et envoyer au client
            return;
        }
        std::cout << "test user4" << std::endl;
    }
    else if (!strncmp(buff, "NICK", 4))
    {
        std::cout << "test nick" << std::endl;
        line = extractMessage(buff + 4);
        if (!line.empty() && !prohibidedCharacter(line))
        {
            if (server.checkDoubleName(line.c_str()))
                client.setNickname(line.c_str());
            else
            {
                server.sendMessage("462" + line + ":You may not reregister\r\n", client);
                return ;
            }
        }
        else
        {
            server.sendMessage("432" + line + ":Erroneous nickname\r\n", client);
            return;
        }
    }
    if (!client.getNickname().empty() && !client.getUsername().empty())
    {
        std::cout << "Nouvelle connexion acceptée. FD: "  << client.getSocket() << std::endl;
        client.onRegisted();
        server.sendMessage("001" + client.getNickname() +  " :Welcome to the Internet Relay Network " + client.getNickname() + "!" + client.getUsername() + "@127.0.0.1", client);
    }
    else
    {
        // server.sendMessage("\r\n", client);
        std::cout << "buu\n";
    }
    std::cout << "setusernick fini0\n";
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
