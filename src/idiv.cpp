#include "idiv.hpp"

int idiv(int a, int b) {
    int r = a / b;
    if (r > 0) return r;
    if (r < 0) return r - 1;
    if (a < 0 ^ b < 0) return r - 1;
    return r;
}

long long idiv(long long a, long long b) {
    long long r = a / b;
    if (r > 0) return r;
    if (r < 0) return r - 1;
    if (a < 0 ^ b < 0) return r - 1;
    return r;
}