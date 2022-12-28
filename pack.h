#ifndef PACK_H
#define PACK_H

void packi16(unsigned char *buf, unsigned int i) {
    *buf++ = i >> 8; *buf++ = i;
}

void packi32(unsigned char *buf, unsigned long int i) {
    *buf++ = i >> 24; *buf++ = i >> 16;
    *buf++ = i >> 8; *buf++ = i;
}

void packi64(unsigned char *buf, unsigned long long int i) {
    *buf++ = i >> 56; *buf++ = i >> 48;
    *buf++ = i >> 40; *buf++ = i >> 32;
    *buf++ = i >> 24; *buf++ = i >> 16;
    *buf++ = i >> 8; *buf++ = i;
}

int unpacki16(unsigned char *buf) {
    unsigned int i = ((unsigned int) buf[0] << 8) | buf[1];

    return i <= 0x7fffu 
           ? i 
           : -1 - (unsigned int)(0xffffu - i);
}

unsigned int unpacku16(unsigned char *buf) {
    return ((unsigned int) buf[0] << 8) | buf[1];
}

long int unpacki32(unsigned char *buf) {
    unsigned long int i = ((unsigned long int) buf[0] << 24) | 
                          ((unsigned long int) buf[1] << 16) |
                          ((unsigned long int) buf[2] << 8)  |
                          buf[3];

    return i <= 0x7fffffffu
           ? i
           : - 1 - (unsigned long int)(0xffffffffu - i);
}

unsigned long int unpacku32(unsigned char *buf) {
    return ((unsigned long int) buf[0] << 24) |
           ((unsigned long int) buf[1] << 16) |
           ((unsigned long int) buf[2] << 8)  |
           buf[3];
}

long long int unpacki64(unsigned char *buf) {
    unsigned long long int i = ((unsigned long long int) buf[0] << 56) |
                               ((unsigned long long int) buf[1] << 48) |
                               ((unsigned long long int) buf[2] << 40) |
                               ((unsigned long long int) buf[3] << 32) |
                               ((unsigned long long int) buf[4] << 24) |
                               ((unsigned long long int) buf[5] << 16) |
                               ((unsigned long long int) buf[6] << 8)  |
                               buf[7];

    return i <= 0x7fffffffffffffffu
           ? i
           : - 1 - (unsigned long long int)(0xfffffffffffffffu - i);
}

unsigned long long int unpacku64(unsigned char *buf) {
    return ((unsigned long long int) buf[0] << 56) |
           ((unsigned long long int) buf[1] << 48) |
           ((unsigned long long int) buf[2] << 40) |
           ((unsigned long long int) buf[3] << 32) |
           ((unsigned long long int) buf[4] << 24) |
           ((unsigned long long int) buf[5] << 16) |
           ((unsigned long long int) buf[6] << 8)  |
           buf[7];
}

#endif // PACK_H
