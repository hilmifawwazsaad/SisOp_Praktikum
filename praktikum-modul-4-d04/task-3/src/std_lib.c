#include "std_lib.h"

int div(int a, int b) {
    // div untuk OS yang tidak memiliki fungsi pembagian
    int quotient = 0;
    while (a >= b) {
        a -= b;
        quotient++;
    }
    return quotient;
}

int mod(int a, int b) {
    // mod untuk OS yang tidak memiliki fungsi modulus
    while (a >= b) {
        a -= b;
    }
    return a;
}

void memcpy(byte* src, byte* dst, unsigned int size) {
    unsigned int i;
    for (i = 0; i < size; i++) {
        dst[i] = src[i];
    }
}

unsigned int strlen(char* str) {
    unsigned int length = 0;
    while (str[length] != '\0') {
        length++;
    }
    return length;
}

bool strcmp(char* str1, char* str2) {
    unsigned int i = 0;
    while (str1[i] != '\0' || str2[i] != '\0') {
        if (str1[i] != str2[i]) {
            return false;
        }
        i++;
    }
    return true;
}

void strcpy(char* src, char* dst) {
    unsigned int i = 0;
    while (src[i] != '\0') {
        dst[i] = src[i];
        i++;
    }
    dst[i] = '\0';
}

void clear(byte* buf, unsigned int size) {
    unsigned int i;
    for (i = 0; i < size; i++) {
        buf[i] = 0;
    }
}
