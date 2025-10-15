#include <Client.hpp>

class Server
{
    private:
        struct sockaddr_in serv_addr;
        int srv = -1;
        static bool Signal;
        std::vector<Client> clients;
        std::vector<pollfd> fds;
        char buffer[512];
    public:
        int createServer();
        struct sockaddr_in &getSockaddr();
        std::vector<pollfd> &getFds();
        std::vector<Client> &getClients();
        void closeAllSockets();
};