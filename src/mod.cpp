#include "mod.hpp"

#include <cmath>

inline int mod(int a, int b) {
    return ((a % b) + b) % b;
}

inline float mod(float a, int b) {
    return fmod((fmod(a, b) + b), b);
}

inline float mod(int a, float b) {
    return fmod((fmod(a, b) + b), b);
}

inline float mod(float a, float b) {
    return fmod((fmod(a, b) + b), b);
}

inline long long mod(long long a, long long b) {
    return ((a % b) + b) % b;
}

inline double mod(double a, int b) {
    return fmod((fmod(a, b) + b), b);
}

inline double mod(int a, double b) {
    return fmod((fmod(a, b) + b), b);
}

inline double mod(double a, long long b) {
    return fmod((fmod(a, b) + b), b);
}

inline double mod(long long a, double b) {
    return fmod((fmod(a, b) + b), b);
}

inline double mod(double a, double b) {
    return fmod((fmod(a, b) + b), b);
}