#include <iostream>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

class Client
{
    private:
        struct sockaddr_in cli_addr;
        std::string ipAddress;
        int cli;
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
    public:
        int createServer();
};

