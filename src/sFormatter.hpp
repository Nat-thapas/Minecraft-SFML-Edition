#ifndef SFORMATTER_HPP
#define SFORMATTER_HPP

#include <cstdlib>
#include <string>

class Formatter {
    std::string baseString;

   public:
    Formatter(std::string baseString);
    std::string formatted(int a);
};

#endif // SFORMATTER_HPP