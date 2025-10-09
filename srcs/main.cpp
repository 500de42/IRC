#include "../includes/IRC.hpp"

int main()
{
    Server server;
    std::string line;
    std::vector<int> ls;

    if (server.createServer())
        return 1;
    while (1)
    {
        poll(&server.getFds()[0], server.getFds().size(), -1);
        std::getline(std::cin , line);
        if (line.empty())
            continue;
    }
    return 0;
}
