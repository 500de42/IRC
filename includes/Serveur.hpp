#include <Client.hpp>

class Server
{
    private:
        struct sockaddr_in serv_addr;
        int srv = -1;
        static bool Signal;
        std::vector<Client> clients;
        std::vector<pollfd> fds;
        char *buffer;
    public:
        int createServer();
        struct sockaddr_in &getSockaddr();
        std::vector<pollfd> &getFds();
        std::vector<Client> &getClients();
        void closeAllSockets();
        void registerClient();
        bool checkDoubleName(const char *name);
        void sendMessage(const char *buffer, Client client);
};

std::string extractMessage(char *tmp);
bool extractAndSetMessageForUser(char *tmp, Client &client, Server &server);
void setUserAndNick(Client &client, Server &server);
std::string joinVector(const std::vector<std::string> &vec, char sep);
bool prohibidedCharacter(std::string tmp);

