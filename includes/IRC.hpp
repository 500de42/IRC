#include <iostream>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

class Client
{
    private:
        std::string ipAddress;
        int Fd;
    public:
        std::string getIpAddr();
        int getFd();
};

class Server
{
    private:
        int srv = -1;
        int Port;
        static bool Signal;
        std::vector<Client> clients;
    public:
        int createServer();
};

