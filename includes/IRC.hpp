#include <iostream>
#include <vector>

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
        int Port;
        static bool Signal;
        std::vector<Client> clients;
    public:
        
};

