#include "./bigNumberUtil.h"
#include <algorithm>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

#define UINT32_BITLEN 32

bool is_binaryStr_valid(const std::string& binaryStr) {
    if(binaryStr.size() == 0)
        return false;

    for(const auto& c : binaryStr) {
        if(c != '0' && c != '1')
            return false;
    }
    return true;
}

bool is_decimalStr_valid(const std::string& decimalStr) {
    if(decimalStr.size() == 0)
        return false;

    for(const auto& c : decimalStr) {
        if(c < '0' || c > '9')
            return false;
    }
    return true;
}

bool is_uint32arr_equal_to_0(const std::vector<uint32_t>& arr) {
    if(arr.size() == 0)
        return false;
    for(const auto& num : arr) {
        if(num != 0)
            return false;
    }
    return true;
}

bool is_decimalStr_equal_to_0(const std::string& decimalStr) {
    if(decimalStr.size() == 0)
        return false;
    for(const auto& num : decimalStr) {
        if(num != '0')
            return false;
    }
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
    if(!is_binaryStr_valid(binaryStr))
        throw std::runtime_error("the binary string \"" + binaryStr + "\" is not valid");

    int arr_size = (binaryStr.size()-1) / UINT32_BITLEN + 1;
    std::vector<uint32_t> arr(arr_size, 0ULL);

    uint32_t mask = 1ULL;
    int num = 0;
    for(int i = binaryStr.size() - 1; i >= 0; i--) {
        if(mask == 0ULL)
            mask = 1ULL;
        if(binaryStr[i] == '1')
            arr[num / UINT32_BITLEN] |= mask;
        mask <<= 1;
        num++;
    }

    trim(arr);

    return arr;
}

std::vector<uint32_t> decimalStr_to_uint32arr(const std::string& decimalStr) {
    if(!is_decimalStr_valid(decimalStr))
        throw std::runtime_error("the decimal string \"" + decimalStr + "\" is not valid");
    std::string binaryStr = decimalStr_to_binaryStr(decimalStr);
    std::vector<uint32_t> arr = binaryStr_to_uint32arr(binaryStr);
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