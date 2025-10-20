#include <iostream>
#pragma once
#include <vector>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string>
#include <sstream>
#include <poll.h>
#include <cstdbool>
#include <cerrno>
#include <Channel.hpp>


class Client
{
    private:
        struct sockaddr_in cli_addr;
        std::string ipAddress;
        int cliSocket = -1;
        int servSocket = -1;
        std::string nickname;
        std::string username;
        std::string realname;
        char *Buffer;
        bool isRegistred = false;
        std::vector<Server::Channel> channels;

    public:
        //      GET     //

        std::string getIpAddr();
        int getFd();
        int getServsocket();
        int getSocket();
        char * getBuffer();
        struct sockaddr_in &getSockaddr();
        bool getRegister();
        std::string &getNickname();
        std::string &getUsername();
        std::string &getRealname();
        std::vector<Server::Channel> &getChannels();

        //      SET     //

        void setServsocket(int nb);
        void setCliSocket(int nb);
        void setBuffer(char *buffer);
        void setNickname(const char *name);
        void setUsername(const char *name);
        void setRealname(const char *name);
        void onRegisted();
        Client();
        //              //

        // Client(bool o);
        Client &operator=(const Client &cpy);
        bool extractMessageForUser(char *tmp);
        void sendMessage(const char *buffer);
};
