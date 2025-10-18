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
        return false;
    if (server.checkDoubleName(tab[1].c_str()))
        client.setUsername(tab[1].c_str());
    else
    {

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

    if (!strncmp(buff, "USER", 4))
    {
        if (client.getUsername().empty())
        {            
            if (!extractAndSetMessageForUser(buff, client, server))
            {    
                // ERR_ALREADYREGISTRED();
                server.sendMessage("\n\r", client);//erreur a ecrire et envoyer au client
            }
        }  
        else
        {
            server.sendMessage("\n\r", client);//erreur a ecrire et envoyer au client
        }
    }
    else if (!strncmp(buff, "NICK", 4))
    {
        line = extractMessage(buff + 4);
        if (!line.empty() && !prohibidedCharacter(line))
            client.setNickname(line.c_str());
    }
    if (!client.getNickname().empty() && !client.getUsername().empty())
        client.onRegisted();
    else
    {
        server.sendMessage("\n\r", client);
    }
}

