#include <iostream>
#include <cstring>
#include <mutex>
#include <thread>
#include <vector>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

std::mutex cout_guard;

namespace Client {
    const int MAXDATASIZE = 100;
} // namespace Client

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void make_connection(int id, char *hostname, char *portnum) {
    int rv;
    int sockfd;
    int numbytes;
    struct addrinfo hints;
    struct addrinfo *servinfo;
    struct addrinfo *p;
    char s[INET6_ADDRSTRLEN];
    char buf[Client::MAXDATASIZE];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(hostname, portnum, &hints, &servinfo)) != 0) {
        std::cerr << "client: getaddrinfo: " << gai_strerror(rv) << "\n";
        return;
    }

    for(p = servinfo; p != nullptr; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            std::cerr << "client: socket failed\n";
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            std::cerr << "client: connect failed\n";
            continue;
        }

        break;
    }

    if (p == nullptr) {
        std::cerr << "client: failed to connect\n";
        return;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof s);

    {
        std::unique_lock guard{cout_guard};
        std::cout << "client " << id << ": connecting to " << s << "\n";
    }

    freeaddrinfo(servinfo);

    if ((numbytes = recv(sockfd, buf, Client::MAXDATASIZE - 1, 0)) == -1) {
        std::cerr << "client: recv failed\n";
        return;
    }

    buf[numbytes] = '\0';

    {
        std::unique_lock guard{cout_guard};
        std::cout << "client " << id << ": received '" << buf << "'\n";
    }

    close(sockfd);
}

int main(int argc, char *argv[])
{
    if (argc != 4) {
        std::cerr << "usage: client <hostname> <port> <number of clients>\n";
        return 1;
    }

    char *hostname = argv[1];
    char *portnum = argv[2];
    int n = std::stoi(argv[3]);

    std::vector<std::thread> threads;
    
    for (int i = 0; i < n; i++) {
        threads.push_back(std::thread(make_connection, i, hostname, portnum));
    }

    for (int i = 0; i < n; i++) {
        threads.at(i).join();
    }

    return 0;
}
