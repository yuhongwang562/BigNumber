#include "./bigNumberUtil.h"
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

#define UINT32_BITLEN 32

bool is_binaryStr_valid(const std::string& binaryStr) {
    if(binaryStr.empty() || binaryStr[0] == '0')
        return false;

    for(const auto& c : binaryStr) 
        if(c != '0' && c != '1')
            return false;
    return true;
}

bool is_decimalStr_valid(const std::string& decimalStr) {
    if(decimalStr.empty() || decimalStr[0] == '0')
        return false;

    for(const auto& c : decimalStr)
        if(c < '0' || c > '9')
            return false;
    return true;
}

bool is_hexStr_valid(const std::string& hexStr) {
    if(hexStr.empty() || hexStr[0] == '0')
        return false;

    for(const auto& c : hexStr)
        if(!((c >= '0' && c <='9') || (c >= 'a' && c <= 'f')))
            return false;
    return true;
}


/*
    需要讓所有數值都僅只有唯一一種 uint32_t 陣列儲存方式
    否則會出現 0 可以是 {0} 也能是 {0, 0} 等不同陣列儲存
    將多餘的 0 捨棄
*/
void trim(std::vector<uint32_t>& arr) {
    while (arr.size() > 1 && arr.back() == 0) {
        arr.pop_back();
    }
    if (arr.empty()) {
        arr.push_back(0);
    }
}

std::vector<uint32_t> binaryStr_to_uint32arr(const std::string& binaryStr) {
    assert(is_binaryStr_valid(binaryStr) == true);

    size_t arr_size = (binaryStr.size()-1) / UINT32_BITLEN + 1;
    std::vector<uint32_t> arr(arr_size, 0);

    uint32_t mask = 1;
    size_t num = 0;
    for(size_t i = binaryStr.size() - 1; i >= 0; i--) {
        mask = mask == 0? 1: mask;
        if(binaryStr[i] == '1')
            arr[num / UINT32_BITLEN] |= mask;
        mask <<= 1;
        num++;

        if(i == 0) break;
    }

    trim(arr);

    return arr;
}

std::vector<uint32_t> decimalStr_to_uint32arr(const std::string& decimalStr) {
    assert(is_decimalStr_valid(decimalStr) == true);

    // 10^n <= 2^k
    // n <= log_10 (2^k)
    // n <= k*log_10 2
    // n <= k*0.3010
    // n*10000 / 3010 <= k
    size_t bits = static_cast<size_t>(decimalStr.size() * 3322ULL) / 1000ULL + 1;
    size_t arr_size = bits / UINT32_BITLEN + 1;

    std::vector<uint32_t> arr(arr_size, 0);

    std::vector<uint32_t> decimalarr;

    return arr;
}

std::string uint32arr_to_decimalStr(std::vector<uint32_t> arr) {
    if(is_uint32arr_equal_to_0(arr))
        return "0";

    std::string res = "";

    uint64_t base = 1ULL << UINT32_BITLEN;
    while(!is_uint32arr_equal_to_0(arr)) {
        uint64_t temp = 0ULL;
        for(int i = arr.size() - 1; i >= 0; i--) {
            temp = temp * base + arr[i];
            arr[i] = temp / 10;
            temp = temp % 10;
        }
        res += std::to_string(temp);
    }
    std::reverse(res.begin(), res.end());

    return res;
}

std::string binaryStr_to_decimalStr(std::string binaryStr) {
    if(!is_binaryStr_valid(binaryStr))
        throw std::runtime_error("the binary string \"" + binaryStr + "\" is not valid");
    std::vector<uint32_t> arr = binaryStr_to_uint32arr(binaryStr);
    std::string res = uint32arr_to_decimalStr(arr);
    return res;
}

std::string decimalStr_to_binaryStr(std::string decimalStr) {
    if(!is_decimalStr_valid(decimalStr))
        throw std::runtime_error("the decimal string \"" + decimalStr + "\" is not valid");
    if(is_decimalStr_equal_to_0(decimalStr))
        return "0";

    std::string res = "";

    while(!is_decimalStr_equal_to_0(decimalStr)) {
        int temp = 0;
        for(int i = 0; i < decimalStr.size(); i++) {
            temp = temp * 10 + (decimalStr[i] - '0');
            decimalStr[i] = '0' + temp / 2;
            temp = temp % 2;
        }
        res += std::to_string(temp);
    }
    std::reverse(res.begin(), res.end());

    return res;
}