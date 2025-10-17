#include "../includes/Serveur.hpp"
#include "../includes/Client.hpp"

int Server::createServer()
{
    this->srv = socket(AF_INET, SOCK_STREAM, 0);
    if (this->srv < 0)
        return 1;
    int opt = 1;
    std::memset(&serv_addr, 0, sizeof(serv_addr));
    this->serv_addr.sin_port = htons(6667);
    this->serv_addr.sin_family = AF_INET;
    this->serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    setsockopt(srv, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if (bind(srv, (struct sockaddr *)&serv_addr, sizeof(serv_addr)))
    {
        std::cerr << "Bind error" << std::endl;
        close(srv);
        return 1;
    }
    std::cout << "Bind ok" << std::endl;
    if (listen(srv, 10))
    {
        std::cerr << "Listen error" << std::endl;
        close(srv);
        return 1;
    }
    struct pollfd server_poll;
    server_poll.fd = srv;
    server_poll.events = POLLIN;
    this->fds.push_back(server_poll);
    this->fds.push_back(server_poll);
    return 0;
}


std::vector<Client> &Server::getClients()
{
    return clients;
}

struct sockaddr_in &Server::getSockaddr()
{
    return this->serv_addr;
}

void Server::closeAllSockets()
{
    for (int i = 0; i <= 11; i++)
    {
        if(this->getClients()[i].getFd() > 0)
            close(this->getClients()[i].getFd());
    }
    if (this->srv)
        close(this->srv);
}

std::vector<std::string> extractMessageForUser(char *tmp)
{
    if (!tmp)
        throw("");
    std::stringstream ss(tmp);
    std::string l;
    std::vector<std::string> tab;
    while (ss >> l)
        tab.push_back(l);
    if (tab.size() != 4)
        throw "";
    return (tab);
}

std::string extractMessage(char *tmp)
{
    std::string line(tmp);
    std::string extract;
    size_t begin = 0, end = 0;

    if (!tmp || line.size() > 8)
        return "";
    begin = line.find_first_of("<", 6);
    if (begin == 0)
    {
        end = line.find_first_of(">", begin);
        if (end != 0)
        extract = line.substr(begin, end);
        return extract;
    }
    return "";
}

bool Server::checkDoubleName(const char *name)
{
    if (!name)
        return false;
    for(int i = 0; i < this->getClients().size(); i++)
    {
        if (name == this->getClients()[i].getUsername())
            return false;
    }
    return true;
}

bool parsingSetUser(std::vector<std::string> &tab)
{
    
}

void setUserAndNick(Client &client, Server &server)
{
    char *buff = client.getBuffer();
    std::string line;
    std::vector<std::string> mess;

    if (!strncmp(buff, "USER", 5))
    {
        if (client.getUsername().empty())
        {            
            mess = extractMessageForUser(buff);
            if (server.checkDoubleName(mess[0].c_str()))
                client.setUsername(line.c_str());
            else
                // ERR_ALREADYREGISTRED();
                std::cout << "\n";//erreur a ecrire
        }
        else
        {
            std::cout << "\n";//erreur a ecrire
        }
    }    
    else if (!strncmp(buff, "NICK", 5))
    {
        line = extractMessage(buff);
        if (!line.empty())
            client.setNickname(line.c_str());
    }
    if (!client.getNickname().empty() && !client.getUsername().empty())
        client.onRegisted();
}
