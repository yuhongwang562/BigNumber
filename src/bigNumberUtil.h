#ifndef BIGNUMBERUTIL_H
#define BIGNUMBERUTIL_H

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <exception>

bool is_binaryStr_valid(const std::string& binaryStr);
bool is_decimalStr_valid(const std::string& decimalStr);

std::vector<uint32_t> binaryStr_to_uint32arr(const std::string& binaryStr);
std::vector<uint32_t> decimalStr_to_uint32arr(const std::string& decimalStr);
std::string uint32arr_to_decimalStr(std::vector<uint32_t> arr);
std::string binaryStr_to_decimalStr(std::string binaryStr);
std::string decimalStr_to_binaryStr(std::string decimalStr);

#endif