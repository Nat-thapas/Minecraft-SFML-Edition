#include "mod.hpp"

#include <cmath>

int mod(int a, int b) {
    int r = a % b;
    return r < 0 ? r + b : r;
}

float mod(float a, int b) {
    float r = fmod(a, b);
    return r < 0 ? r + b : r;
}

float mod(int a, float b) {
    float r = fmod(a, b);
    return r < 0 ? r + b : r;
}

float mod(float a, float b) {
    float r = fmod(a, b);
    return r < 0 ? r + b : r;
}

long long mod(long long a, long long b) {
    long long r = a % b;
    return r < 0 ? r + b : r;
}

double mod(double a, int b) {
    double r = fmod(a, b);
    return r < 0 ? r + b : r;
}

double mod(int a, double b) {
    double r = fmod(a, b);
    return r < 0 ? r + b : r;
}

double mod(double a, long long b) {
    double r = fmod(a, b);
    return r < 0 ? r + b : r;
}

double mod(long long a, double b) {
    double r = fmod(a, b);
    return r < 0 ? r + b : r;
}

double mod(double a, double b) {
    double r = fmod(a, b);
    return r < 0 ? r + b : r;
}