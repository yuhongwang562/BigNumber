#include "../src/binaryBigInt.h"
#include <iostream>
#include <string>
#include <exception>

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    std::string cmd;
    while (std::cin >> cmd) {
        if (cmd == "EXIT") {
            break;
        } else if (cmd == "DEFAULT") {
            try {
                BinaryBigInt n;
                std::cout << "OK " << to_string(n) << " "
                          << (n.is_zero() ? 1 : 0) << " "
                          << (n.is_negative() ? 1 : 0) << " "
                          << n.bit_length() << " "
                          << n.limb() << "\n";
            } catch (const std::exception& e) {
                std::cout << "ERR " << e.what() << "\n";
            }
        } else if (cmd == "NEW") {
            std::string s;
            std::cin >> s;
            try {
                BinaryBigInt n(s);
                std::cout << "OK " << to_string(n) << " "
                          << (n.is_zero() ? 1 : 0) << " "
                          << (n.is_negative() ? 1 : 0) << " "
                          << n.bit_length() << " "
                          << n.limb() << "\n";
            } catch (const std::exception& e) {
                std::cout << "ERR " << e.what() << "\n";
            }
        } else {
            std::cout << "UNKNOWN\n";
        }
        std::cout << std::flush;
    }
    return 0;
}
