#include "sFormatter.hpp"

#include <format>

Formatter::Formatter(std::string baseString) {
    this->baseString = baseString;
}

std::string Formatter::formatted(int a) {
    return std::format(baseString, a);
}