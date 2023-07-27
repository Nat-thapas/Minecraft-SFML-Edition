#include <iostream>
#include <cstdlib>
#include <string>
#include <format>

class Formatter {
    static constexpr std::string baseString = "Hi {}";

   public:
    std::string formatted(int a) {
        return std::format(baseString, a);
    }
};

int main() {
    Formatter formatter;
    std::cout << formatter.formatted(42);
}