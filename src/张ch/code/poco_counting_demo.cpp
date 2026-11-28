#include <Poco/CountingStream.h>
#include <fstream>
#include <iostream>
#include <string>

int main() {
    std::ifstream file("input.txt");
    if (!file) {
        std::cerr << "cannot open input.txt\n";
        return 1;
    }

    Poco::CountingInputStream in(file);

    std::string line;
    while (std::getline(in, line)) {
        std::cout << line << '\n';
    }

    std::cout << "chars=" << in.chars()
              << " lines=" << in.lines()
              << " pos=" << in.pos() << '\n';
    return 0;
}
