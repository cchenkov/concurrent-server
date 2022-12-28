#include "pack.h"

#include <iostream>

int main() {
    unsigned char buf[8];

    int i = -32;

    packi16(buf, i);

    std::cout << "Packed: " << i << "\n"
              << "Unpacked: " << unpacki16(buf) << "\n";

    return 0;
}
