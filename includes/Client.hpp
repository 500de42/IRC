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
        int cliSocket = -1;
        int servSocket = -1;
        std::string line;
        std::string nickname;
        std::string username;
        char buffer[512];
    public:
        std::string getIpAddr();
        int getFd();
        int getServsocket();
        int getSocket();
        void setServsocket(int nb);
        Client createClient();
        Client();
};