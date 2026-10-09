#include "../src/bigNumberUtil.h"
#include <iostream>
#include <string>
#include <vector>

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    std::string cmd;
    while (std::cin >> cmd) {
        if (cmd == "EXIT") {
            break;
        } 
        else if (cmd == "DEC") {
            std::string s;
            std::cin >> s;
            auto arr = decimalStr_to_uint32arr(s);
            std::cout << uint32arr_to_BaseStr(arr, 10) << "\n";
        } 
        else if (cmd == "BIN") {
            std::string s;
            std::cin >> s;
            auto arr = binaryStr_to_uint32arr(s);
            std::cout << uint32arr_to_BaseStr(arr, 2) << "\n";
        } 
        else if (cmd == "HEX") {
            std::string s;
            std::cin >> s;
            auto arr = HexStr_to_uint32arr(s);
            std::cout << uint32arr_to_BaseStr(arr, 16) << "\n";
        } 
        else if (cmd == "BASE") {
            std::string s;
            int base;
            std::cin >> s >> base;
            auto arr = decimalStr_to_uint32arr(s);
            std::cout << uint32arr_to_BaseStr(arr, base) << "\n";
        } 
        else if (cmd == "CMP") {
            std::string a, b;
            std::cin >> a >> b;
            auto arr_a = decimalStr_to_uint32arr(a);
            auto arr_b = decimalStr_to_uint32arr(b);
            std::cout << compare_magnitude(arr_a, arr_b) << "\n";
        } 
        else if (cmd == "ADD") {
            std::string a, b;
            std::cin >> a >> b;
            auto arr_a = decimalStr_to_uint32arr(a);
            auto arr_b = decimalStr_to_uint32arr(b);
            std::vector<uint32_t> res;
            add_magnitude(res, arr_a, arr_b);
            std::cout << uint32arr_to_BaseStr(res, 10) << "\n";
        } 
        else if (cmd == "ADD_ALIAS_A") {
            std::string a, b;
            std::cin >> a >> b;
            auto arr_a = decimalStr_to_uint32arr(a);
            auto arr_b = decimalStr_to_uint32arr(b);
            add_magnitude(arr_a, arr_a, arr_b);
            std::cout << uint32arr_to_BaseStr(arr_a, 10) << "\n";
        } 
        else if (cmd == "ADD_ALIAS_SELF") {
            std::string a;
            std::cin >> a;
            auto arr_a = decimalStr_to_uint32arr(a);
            add_magnitude(arr_a, arr_a, arr_a);
            std::cout << uint32arr_to_BaseStr(arr_a, 10) << "\n";
        } 
        else if (cmd == "SUB") {
            std::string a, b;
            std::cin >> a >> b;
            auto arr_a = decimalStr_to_uint32arr(a);
            auto arr_b = decimalStr_to_uint32arr(b);
            std::vector<uint32_t> res;
            sub_magnitude(res, arr_a, arr_b);
            std::cout << uint32arr_to_BaseStr(res, 10) << "\n";
        } 
        else if (cmd == "SUB_ALIAS_A") {
            std::string a, b;
            std::cin >> a >> b;
            auto arr_a = decimalStr_to_uint32arr(a);
            auto arr_b = decimalStr_to_uint32arr(b);
            sub_magnitude(arr_a, arr_a, arr_b);
            std::cout << uint32arr_to_BaseStr(arr_a, 10) << "\n";
        } 
        else if (cmd == "SUB_ALIAS_SELF") {
            std::string a;
            std::cin >> a;
            auto arr_a = decimalStr_to_uint32arr(a);
            sub_magnitude(arr_a, arr_a, arr_a);
            std::cout << uint32arr_to_BaseStr(arr_a, 10) << "\n";
        } 
        else {
            std::cerr << "Unknown command: " << cmd << "\n";
        }
        std::cout << std::flush;
    }
    return 0;
}
