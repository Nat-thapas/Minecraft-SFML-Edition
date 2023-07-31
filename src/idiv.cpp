#include "idiv.hpp"

inline int idiv(int a, int b) {
    return (a - (((a % b) + b) % b)) / b;
}

inline long long idiv(long long a, long long b) {
    return (a - (((a % b) + b) % b)) / b;
}