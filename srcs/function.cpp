#include "../includes/Serveur.hpp"
#include "../includes/Client.hpp"

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
    if (tab.size() > 5)
        return false;
    if (tab[4][0] != ':')
        return false;
    tab[4].erase(0, 1);
    if (tab.size() < 5)
    {
        std::vector<std::string> vec(tab.begin() + 4, tab.end());
        l = joinVector(vec, ' ');
        tab[4] = l;
    }
    if (prohibidedCharacter(tab[1]) || prohibidedCharacter(tab[4]))
    {
        server.sendMessage("461" + tab[1] + ":Not enough parameters\r\n", client);   
        return false;
    }
    // if (server.checkDoubleName(tab[1].c_str()))
    if (client.getUsername().empty())
    {
        client.setUsername(tab[1].c_str());
        client.setRealname(tab[4].c_str());
    }
    else
    {
        server.sendMessage("462" + tab[1] + ":You may not reregister\r\n", client);
        return false;
    }
    return (true);
}

bool prohibidedCharacter(std::string tmp)
{
    for (std::string::iterator i = tmp.begin(); i != tmp.end(); i++)
    {
        if (*i == ' ' || *i == ',' || *i == '*' || *i == '?' 
            || *i == '!' || *i == '@' || *i >= 0 && *i <= 31)
        {
            std::cout << ":server_name 461 * USER :Not enough parameters\n";
            return true;
        }
    }
    return false;
}

template <typename T>
void sendMessage(T c, std::string buffer)
{
    T.send
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

bool parsingSetUser(std::vector<std::string> &tab)
{
    
}

void setUserAndNick(Client &client, Server &server)
{
    char *buff = client.getBuffer();
    std::string line;
    std::vector<std::string> mess;

    if (strncmp(buff, "USER", 4) && strncmp(buff, "NICK", 4))
    {
        server.sendMessage("451 " + (std::string)buff + " :You have not registered\r\n", client); return;
    }
    if (!strncmp(buff, "USER", 4))
    {
        if (client.getUsername().empty())          
            if (!extractAndSetMessageForUser(buff, client, server))
            {    
                // ERR_ALREADYREGISTRED();
                server.sendMessage("\r\n", client);//erreur a ecrire et envoyer au client
            }
        else
            server.sendMessage("\r\n", client);//erreur a ecrire et envoyer au client
    }
    else if (!strncmp(buff, "NICK", 4))
    {
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
        client.onRegisted();
        server.sendMessage("001" + client.getNickname() +  " :Welcome to the Internet Relay Network " + client.getNickname() + "!" + client.getUsername() + "@127.0.0.1", client);
    }
    else
        server.sendMessage("\r\n", client);
}

