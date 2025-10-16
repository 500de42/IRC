#include <iostream>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string>
#include <sstream>
#include <poll.h>
#include <cstdbool>

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
        char Buffer[512];
        Client();
    public:
        //      GET     //

        std::string getIpAddr();
        int getFd();
        int getServsocket();
        int getSocket();
        char *getBuffer();
        struct sockaddr_in &getSockaddr();

        //      SET     //

        void setServsocket(int nb);
        void setCliSocket(int nb);
        void setBuffer(char *buffer);

        //              //

        Client(bool o);

};