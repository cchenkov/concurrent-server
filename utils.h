#include "constants.h"
#include "pack.h"
#include "strutils.h"

#include <sys/types.h>
#include <sys/socket.h>

#include <fmt/core.h>

void create_packet(char *data, int len, char *out) {
    unsigned char packet_len[PACKET_LENGTH_BYTES];
    packi32(packet_len, len);

    strcopy(out, (char *)packet_len, PACKET_LENGTH_BYTES);
    strconcat(out, PACKET_LENGTH_BYTES, data, len);
}

int sendall(int sock, char *buf, int *len) {
    int total = 0;
    int bytes_left = *len;
    int bytes_sent;

    while (total < *len) {
        bytes_sent = send(sock, buf + total, bytes_left, 0);
        
        if (bytes_sent == -1) {
            break;
        }

        total += bytes_sent;
        bytes_left -= bytes_sent;
    }

    *len = total;

    return bytes_sent == -1 ? -1 : 0;
}

int recvall(int sock, char *buf, int *len) {
    int total = 0;
    int bytes_recv;
    int bytes_left = PACKET_LENGTH_BYTES;
    unsigned char packet_len[PACKET_LENGTH_BYTES];

    while (total < PACKET_LENGTH_BYTES) {
        bytes_recv = recv(sock, packet_len + total, bytes_left, 0);

        if (bytes_recv == -1) {
            return -1;
        }

        total += bytes_recv;
        bytes_left -= bytes_recv;
    }

    int packet_size = unpacki32(packet_len);

    total = 0;
    bytes_left = packet_size;

    while (total < packet_size) {
        bytes_recv = recv(sock, buf + total, bytes_left, 0);

        if (bytes_recv == -1) {
            return -1;
        }

        total += bytes_recv;
        bytes_left -= bytes_recv;
    }

    *len = total;

    return 0;
}
