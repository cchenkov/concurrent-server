#include "constants.h"
#include "thread_pool.h"
#include "pack.h"
#include "utils.h"
#include "strutils.h"
#include "quicksort.h"

#include <iostream>
#include <cstring>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#include <fmt/core.h>

void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "usage: server <port>\n";
        return 1;
    }

    char *portnum = argv[1];

    if (!is_numeric(portnum)) {
        std::cerr << "server: invalid port number\n";
        return 1;
    }

    int rv;
    int sockfd;
    struct addrinfo hints;
    struct addrinfo *servinfo;
    struct addrinfo *p;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((rv = getaddrinfo(nullptr, portnum, &hints, &servinfo)) != 0) {
        std::cerr << "server: getaddrinfo: " << gai_strerror(rv) << "\n";
        return 1;
    }

    for (p = servinfo; p != nullptr; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            std::cerr << "server: socket failed\n";
            continue;
        }

        int opt = 1;

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof opt) == -1) {
            std::cerr << "server: setsockopt failed\n";
            return 1;
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            std::cerr << "server: bind failed\n";
            continue;
        }
    
        break;
    }

    freeaddrinfo(servinfo);

    if (p == nullptr) {
        std::cerr << "server: failed to bind\n";
        return 2;
    }

    if (listen(sockfd, DEFAULT_BACKLOG) == -1) {
        std::cerr << "server: listen failed\n";
        return 1;
    }

    thread_pool tp;

    struct sockaddr_storage their_addr;
    socklen_t sin_size;
    int newfd;
    char s[INET6_ADDRSTRLEN];

    while (true) {
        sin_size = sizeof their_addr;
        newfd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);

        if (newfd == -1) {
            std::cerr << "server: accept failed\n";
            continue;
        }

        inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);

        fmt::print("server: got connection from {}\n", s);

        tp.enqueue_work([=]() {
            char buf[MAX_PACKET_SIZE];
            int len;

            if (recvall(newfd, buf, &len) == -1) {
                std::cerr << "server: recvall failed\n";
            } else {
                int size = unpacki32((unsigned char *)buf);

                std::vector<int> vec;
                vec.reserve(size);

                std::cout << "server: received ";

                for (int i = ARRAY_LENGTH_BYTES; i < len; i += 2) {
                    vec.push_back(unpacki16((unsigned char *)(buf + i)));
                    std::cout << vec.back() << " ";
                }

                std::cout << "\n";

                qsort_seq(vec.begin(), vec.end());

                std::cout << "server: sorted ";

                for (std::size_t i = 0; i < vec.size(); i++) {
                    std::cout << vec[i] << " ";
                }

                std::cout << "\n";

                int data_size = size * 2 + ARRAY_LENGTH_BYTES;
                int res_len = data_size + PACKET_LENGTH_BYTES;
                char *data = new char[data_size];
                char *res = new char[res_len];
                
                pack_array(vec.data(), vec.size(), data);
                create_packet(data, data_size, res);

                if (sendall(newfd, res, &res_len) == -1) {
                    std::cerr << "server: sendall failed\n";
                } 

                delete[] data;
                delete[] res;
            }

            close(newfd);
        });
    }

    return 0;
}
