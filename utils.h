#include "constants.h"
#include "pack.h"
#include "strutils.h"

#include <vector>

#include <sys/types.h>
#include <sys/socket.h>

#include <fmt/core.h>

void create_packet(char *data, int len, char *out) {
    unsigned char packet_len[PACKET_LENGTH_BYTES];
    packi32(packet_len, len);

    strcopy(out, (char *) packet_len, PACKET_LENGTH_BYTES);
    strconcat(out, PACKET_LENGTH_BYTES, data, len);
}

std::vector<char> pack_vec(std::vector<int> &vec) {
    std::vector<char> res;
    res.reserve(ARRAY_LENGTH_BYTES + vec.size() * 2);

    unsigned char size[ARRAY_LENGTH_BYTES];
    packi32(size, vec.size());

    res.insert(res.begin(), size, size + ARRAY_LENGTH_BYTES);

    for (int i = 0; i < vec.size(); i++) {
        unsigned char n[2];
        packi16(n, vec[i]);
        res.insert(res.end(), n, n + 2);
    }

    return res;
}

std::vector<int> unpack_vec(std::vector<char> &buf) {
    int size = unpacki32((unsigned char *)buf.data());

    std::vector<int> vec;
    vec.reserve(size);

    for (int i = ARRAY_LENGTH_BYTES; i < buf.size(); i += 2) {
        int n = unpacki16((unsigned char *)(buf.data() + i));
        vec.push_back(n);
    }

    return vec;
}

void pack_array(int *arr, int len, char *out) {
    unsigned char arr_len[ARRAY_LENGTH_BYTES];
    packi32(arr_len, len);

    strcopy(out, (char *)arr_len, ARRAY_LENGTH_BYTES);

    for (int i = 0; i < len; i++) {
        unsigned char curr[2];
        packi16(curr, arr[i]);
        strconcat(out, ARRAY_LENGTH_BYTES + i * 2, (char *) curr, 2);
    }
}

void unpack_array(char *in, int *arr, int *len) {
    *len = unpacki32((unsigned char *)in);
    arr = new int[*len];
    
    for (int i = ARRAY_LENGTH_BYTES, idx = 0; i < *len; i += 2) {
        arr[idx++] = unpacki16((unsigned char *)(in + i));
    }
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
