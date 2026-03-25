#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctime>
#include <sstream>
#include <cstdlib>
#include <sys/signal.h>
#include <csignal>

volatile sig_atomic_t g_running = 1;

void signal_handler(int sig) 
{
    (void)sig; 
    g_running = 0;
}

void setup_signal_handlers()
{
    signal(SIGINT, signal_handler);
    signal(SIGQUIT, signal_handler);
    signal(SIGTERM, signal_handler);
}


class Bot
{
    private:
        std::string server;
        int port;
        std::string pass;
        std::string nick;
        std::string channel;
        int sock;

        void sendMsg(std::string msg)
        {
            msg += "\r\n";
            send(sock, msg.c_str(), msg.length(), 0);
            std::cout << ">> " << msg;
        }

        std::string getTime()
        {
            time_t now = time(0);
            char buf[80];
            strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now));
            return std::string(buf);
        }

        std::string getDate()
        {
            time_t now = time(0);
            char buf[80];
            strftime(buf, sizeof(buf), "%A, %B %d %Y", localtime(&now));
            return std::string(buf);
        }

    public:
        Bot(std::string srv, int p, std::string pw, std::string n, std::string ch)
            : server(srv), port(p), pass(pw), nick(n), channel(ch), sock(-1) {}

        bool connect()
        {
            sock = socket(AF_INET, SOCK_STREAM, 0);
            if (sock < 0)
            {
                std::cerr << "Socket creation failed\n";
                return false;
            }

            struct sockaddr_in serv_addr;
            serv_addr.sin_family = AF_INET;
            serv_addr.sin_port = htons(port);
            
            if (inet_pton(AF_INET, server.c_str(), &serv_addr.sin_addr) <= 0)
            {
                std::cerr << "Invalid address\n";
                return false;
            }

            if (::connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
            {
                std::cerr << "Connection failed\n";
                return false;
            }

            std::cout << "[+] Connected to " << server << ":" << port << std::endl;

            sendMsg("PASS " + pass);
            usleep(300000);
            sendMsg("NICK " + nick);
            usleep(300000);
            sendMsg("USER " + nick + " 0 * :TimeBot");
            usleep(500000);
            sendMsg("JOIN " + channel);
            usleep(300000);

            std::cout << "[+] Joined " << channel << std::endl;
            return true;
        }

        void run()
        {
            char buffer[512];
            time_t lastUpdate = time(0);

            sendMsg("PRIVMSG " + channel + " :Hello! Ask me about time or date");

            while (g_running)
            {
                memset(buffer, 0, sizeof(buffer));
                int n = recv(sock, buffer, sizeof(buffer) - 1, MSG_DONTWAIT);
                
                if (n > 0)
                {
                    std::string msg(buffer);
                    std::cout << "<< " << msg;

                    if (msg.find("PING") == 0)
                    {
                        std::string pong = msg;
                        size_t pos = pong.find("PING");
                        pong.replace(pos, 4, "PONG");
                        sendMsg(pong.substr(0, pong.find("\r\n")));
                    }

                    if (msg.find("PRIVMSG") != std::string::npos && 
                        msg.find(channel) != std::string::npos)
                    {
                        std::string lower = msg;
                        for (size_t i = 0; i < lower.length(); i++)
                            lower[i] = tolower(lower[i]);

                        if (lower.find("time") != std::string::npos)
                        {
                            sendMsg("PRIVMSG " + channel + " :Current time: " + getTime());
                        }
                        if (lower.find("date") != std::string::npos)
                        {
                            sendMsg("PRIVMSG " + channel + " :Date: " + getDate());
                        }
                    }
                }
                else if(n == 0)
                    if (sock >= 0)
                        close(sock);

                time_t now = time(0);
                if (now - lastUpdate >= 300)
                {
                    sendMsg("PRIVMSG " + channel + " :=== Auto Update ===");
                    sendMsg("PRIVMSG " + channel + " :Time: " + getTime());
                    sendMsg("PRIVMSG " + channel + " :Date: " + getDate());
                    lastUpdate = now;
                }

                usleep(100000);
            }
            if (sock >= 0)
            {
                sendMsg("QUIT :Bot shutting down");
                close(sock);
            }
        }

        ~Bot()
        {
            if (sock >= 0)
            {
                sendMsg("QUIT :Bot shutting down");
                close(sock);
            }
        }
};

int main(int ac, char **av)
{
	if (ac < 3)
	{
		std::cerr << "Usage: " << av[0] << " <port> <password> [nick] [channel]\n";
		return 1;
	}

	std::string server = "127.0.0.1";
	int port = atoi(av[1]);
	std::string pass = av[2];
	std::string nick = (ac > 3) ? av[3] : "TimeBot";
	std::string channel = (ac > 4) ? av[4] : "#timebot";

	Bot bot(server, port, pass, nick, channel);
	
	if (!bot.connect())
		return 1;
    setup_signal_handlers();
	bot.run();
	return 0;
}