#include "../includes/IRC.hpp"

int Server::createServer()
{
    this->srv = socket(AF_INET, SOCK_STREAM, 0);
    if (this->srv < 0)
        return 1;

    std::memset(&serv_addr, 0, sizeof(serv_addr));
    this->serv_addr.sin_port = htons(6667);
    this->serv_addr.sin_family = AF_INET;
    this->serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(srv, (struct sockaddr *)&serv_addr, sizeof(serv_addr)))
    {
        std::cerr << "Bind error" << std::endl;
        close(srv);
        return 1;
    }
    std::cout << "Bind ok" << std::endl;
    close(srv);
    if (listen(srv, 10))
    {
        std::cerr << "Listen error" << std::endl;
        close(srv);
        return 1;
    }
    // this->clients.push_back()
}

Client Client::createClient()
{
    this->cli = socket(AF_INET, SOCK_STREAM, 0);
    if (this->cli < 0)
        return 0;
    std::memset(&cli_addr, 0, sizeof(cli_addr));
    this->cli_addr.sin_port = htons(6667);
    this->cli_addr.sin_family = AF_INET;
    this->cli_addr.sin_addr.s_addr = inet_addr("127.0.1.0");
    return *this;
}
