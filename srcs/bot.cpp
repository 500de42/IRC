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
#include <cctype>

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

        std::string toLowerStr(const std::string &input)
        {
            std::string out = input;
            for (size_t i = 0; i < out.length(); i++)
                out[i] = static_cast<char>(tolower(static_cast<unsigned char>(out[i])));
            return out;
        }

    public:
        Bot(std::string srv, int p, std::string pw, std::string n)
            : server(srv), port(p), pass(pw), nick(n), channel("#timebot"), sock(-1) {}

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
            std::string recvBuffer;
            time_t lastUpdate = time(0);

            while (g_running)
            {
                memset(buffer, 0, sizeof(buffer));
                int n = recv(sock, buffer, sizeof(buffer) - 1, MSG_DONTWAIT);
                
                if (n > 0)
                {
                    std::string chunk(buffer, n);
                    std::cout << "<< " << chunk;

                    recvBuffer += chunk;
                    size_t eol = 0;
                    while ((eol = recvBuffer.find("\r\n")) != std::string::npos)
                    {
                        std::string line = recvBuffer.substr(0, eol);
                        recvBuffer.erase(0, eol + 2);

                        if (line.empty())
                            continue;

                        if (line.find("PING") == 0)
                        {
                            sendMsg("PONG" + line.substr(4));
                            continue;
                        }

                        size_t privmsgPos = line.find(" PRIVMSG ");
                        if (privmsgPos == std::string::npos)
                            continue;

                        size_t targetStart = privmsgPos + 9;
                        size_t targetEnd = line.find(' ', targetStart);
                        if (targetEnd == std::string::npos)
                            continue;

                        std::string target = line.substr(targetStart, targetEnd - targetStart);
                        if (target != channel && target != nick)
                            continue;

                        size_t textPos = line.find(" :", targetEnd);
                        if (textPos == std::string::npos)
                            continue;

                        std::string senderNick;
                        if (!line.empty() && line[0] == ':')
                        {
                            size_t exclPos = line.find('!');
                            if (exclPos != std::string::npos && exclPos > 1)
                                senderNick = line.substr(1, exclPos - 1);
                        }
                        if (senderNick == nick)
                            continue;

                        std::string text = toLowerStr(line.substr(textPos + 2));
                        std::string replyTarget = channel;
                        if (target == nick && !senderNick.empty())
                            replyTarget = senderNick;

                        if (text.find("time") != std::string::npos ||
                            text.find("!time") != std::string::npos ||
                            text.find("heure") != std::string::npos)
                            sendMsg("PRIVMSG " + replyTarget + " :Current time: " + getTime());
                        if (text.find("date") != std::string::npos ||
                            text.find("!date") != std::string::npos)
                            sendMsg("PRIVMSG " + replyTarget + " :Date: " + getDate());
                    }
                }
                else if(n == 0)
                {
                    if (sock >= 0)
                        close(sock);
                    break;
                }

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
        std::cerr << "Usage: " << av[0] << " <port> <password> [nick]\n";
		return 1;
	}

	std::string server = "127.0.0.1";
	int port = atoi(av[1]);
	std::string pass = av[2];
	std::string nick = (ac > 3) ? av[3] : "TimeBot";

    Bot bot(server, port, pass, nick);
	
	if (!bot.connect())
		return 1;
    setup_signal_handlers();
	bot.run();
	return 0;
}