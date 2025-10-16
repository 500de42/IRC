#include "../includes/Serveur.hpp"
#include "../includes/Client.hpp"

int main()
{
    try
    {
        Client client(true);
        std::string line;


        std::getline(std::cin , line);

    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}
