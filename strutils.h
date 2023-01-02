#ifndef STRUTILS_H
#define STRUTILS_H

#include <iostream>
#include <cctype>

char *strcopy(char *dst, char *src, int srclen) {
    if (dst == nullptr) {
        return nullptr;
    }
    
    for (int i = 0; i < srclen; i++) {
        dst[i] = src[i];
    }

    return dst;
}

char *strconcat(char *dst, int dstlen, char *src, int srclen) {
    char *ptr = dst + dstlen;

    for (int i = 0; i < srclen; i++) {
        *ptr++ = src[i];
    }

    return dst;
}

bool is_numeric(char *str) {
    while (*str) {
        if (!std::isdigit(*str++)) {
            return false;
        }
    }

    return true;
}

#endif // STRUTILS_H
