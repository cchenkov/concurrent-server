#include "constants.h"
#include "thread_pool.h"
#include "pack.h"
#include "utils.h"

#include <iostream>
#include <cstring>
#include <mutex>
#include <unistd.h>
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

void make_connection(int id, char *hostname, char *portnum) {
    int rv;
    int sockfd;
    int numbytes;
    struct addrinfo hints;
    struct addrinfo *servinfo;
    struct addrinfo *p;
    char s[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(hostname, portnum, &hints, &servinfo)) != 0) {
        std::cerr << "client: getaddrinfo: " << gai_strerror(rv) << "\n";
        return;
    }

    for (p = servinfo; p != nullptr; p = p->ai_next) {
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

    fmt::print("client {}: connecting to {}\n", id, s);

    freeaddrinfo(servinfo);

    // construct data packet
    int arr[5] = { 7, 6, 9, 1, 4 };
    char data[14];
    unsigned char arr_len[ARRAY_SIZE_BYTES];
    packi32(arr_len, 5);

    int i = 0;

    for (; i < ARRAY_SIZE_BYTES; i++) {
        data[i] = arr_len[i];
    }

    i = ARRAY_SIZE_BYTES;

    for (int j = 0; j < 5; j++) {
        unsigned char num[2];
        packi16(num, arr[j]);
        data[i] = num[0];
        data[i + 1] = num[1];
        i += 2;
    }

    char buf[18];
    create_packet(data, 14, buf);

    int len = 18;

    if (sendall(sockfd, buf, &len) == -1) {
        std::cerr << "client: send failed\n";
    }

    close(sockfd);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        std::cerr << "usage: client <hostname> <port> <number of clients>\n";
        return 1;
    }

    char *hostname = argv[1];
    char *portnum = argv[2];
    int n = std::stoi(argv[3]);
   
    thread_pool tp;

    for (int i = 0; i < n; i++) {
        tp.enqueue_work(make_connection, i, hostname, portnum);
    }

    return 0;
}
