#include <cmath>
#include "cmod.hpp"

int cmod(int a, int b) {
    int r = a % b;
    return r < 0 ? r + b : r;
}

float cmod(float a, int b) {
    float r = fmod(a, b);
    return r < 0 ? r + b : r;
}

float cmod(int a, float b) {
    float r = fmod(a, b);
    return r < 0 ? r + b : r;
}

float cmod(float a, float b) {
    float r = fmod(a, b);
    return r < 0 ? r + b : r;
}

long long cmod(long long a, long long b) {
    long long r = a % b;
    return r < 0 ? r + b : r;
}

double cmod(double a, int b) {
    double r = fmod(a, b);
    return r < 0 ? r + b : r;
}

double cmod(int a, double b) {
    double r = fmod(a, b);
    return r < 0 ? r + b : r;
}

double cmod(double a, long long b) {
    double r = fmod(a, b);
    return r < 0 ? r + b : r;
}

double cmod(long long a, double b) {
    double r = fmod(a, b);
    return r < 0 ? r + b : r;
}

double cmod(double a, double b) {
    double r = fmod(a, b);
    return r < 0 ? r + b : r;
}