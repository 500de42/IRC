#include <iostream>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string>
#include <sstream>
#include <poll.h>

class Client
{
    private:
        struct sockaddr_in cli_addr;
        std::string ipAddress;
        int cli = -1;
        std::string nickname;
        std::string username;
        std::string buffer;
    public:
        std::string getIpAddr();
        int getFd();
        Client createClient();
};

class Server
{
    private:
        struct sockaddr_in serv_addr;
        int srv = -1;
        static bool Signal;
        std::vector<Client> clients;
        std::vector<pollfd> fds;
    public:
        int createServer();
        std::vector<pollfd> getFds();
};

std::
