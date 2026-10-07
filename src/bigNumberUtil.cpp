#include "./bigNumberUtil.h"
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#define UINT32_BITLEN 32

bool is_binaryStr_valid(const std::string& binaryStr) {
    if(binaryStr.empty() || (binaryStr.size() > 1 && binaryStr[0] == '0'))
        return false;

    for(const auto& c : binaryStr) 
        if(c != '0' && c != '1')
            return false;
    return true;
}

bool is_decimalStr_valid(const std::string& decimalStr) {
    if(decimalStr.empty() || (decimalStr.size() > 1 && decimalStr[0] == '0'))
        return false;

    for(const auto& c : decimalStr)
        if(c < '0' || c > '9')
            return false;
    return true;
}

bool is_hexStr_valid(const std::string& hexStr) {
    if(hexStr.empty() || (hexStr.size() > 1 && hexStr[0] == '0'))
        return false;

    for(const auto& c : hexStr)
        if(!((c >= '0' && c <='9') || (c >= 'a' && c <= 'f')))
            return false;
    return true;
}

std::vector<uint32_t> decimalStr_to_billionBaseArr(const std::string& decimalStr) {
    size_t arr_size = (decimalStr.size()-1)/9 + 1;
    std::vector<uint32_t> decimalarr(arr_size, 0);
    for(size_t i = 0; i < arr_size; i++) {
        size_t start = (decimalStr.size()-1)-9*i;
        size_t end = start - 8;
        if(end > start)
            end = 0;
        
        uint32_t temp = 0;
        for(size_t j = end; j <= start; j++) {
            temp = temp*10 + (decimalStr[j] - '0');
        }
        decimalarr[i] = temp;
    }
    return decimalarr;
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
    std::vector<uint32_t> decimalarr = decimalStr_to_billionBaseArr(decimalStr);

    size_t num = 0;
    uint32_t mask = 1;
    // decimalarr 是否等於 0 向量
    bool flag = false;
    while(!flag) {
        flag = true;
        uint64_t temp = 0;
        uint64_t base = 1000000000; // 十億進制下的基數
        for(size_t i = decimalarr.size() - 1; i >= 0; i--) {
            temp = temp * base + decimalarr[i];
            decimalarr[i] = temp / 2;
            temp = temp % 2;

            if(decimalarr[i] != 0)
                flag = false;
            if(i == 0) break;
        }

        if(temp == 1ULL)
            arr[num / UINT32_BITLEN] |= mask;
        num++;
        mask <<= 1;
        if(mask == 0)
            mask = 1;
    }

    trim(arr);

    return arr;
}

std::vector<uint32_t> HexStr_to_uint32arr(const std::string& hexStr) {
    assert(is_hexStr_valid(hexStr));

    size_t bits = hexStr.size() * 4;
    size_t arr_size = bits / UINT32_BITLEN + 1;

    std::vector<uint32_t> arr(arr_size, 0);
    size_t num = 0;
    for(size_t i = hexStr.size()-1; i >= 0; i--) {
        uint32_t hex = 0;
        if(hexStr[i] >= '0' && hexStr[i] <= '9')
            hex = hexStr[i] - '0';
        else
            hex = hexStr[i] - 'a' + 10;

        arr[num / 8] |= (hex << (4 * (num % 8)));
        num++;

        if(i == 0) break;
    }

    trim(arr);

    return arr;
}

std::string uint32arr_to_decimalStr(std::vector<uint32_t> arr) {
    if(arr.size() == 1 && arr[0] == 0)
        return "0";

    std::string res = "";

    uint64_t base = 1ULL << UINT32_BITLEN;
    bool flag = false;
    while(!flag) {
        flag = true;
        uint64_t temp = 0ULL;
        for(size_t i = arr.size() - 1; i >= 0; i--) {
            temp = temp * base + arr[i];
            arr[i] = temp / 10;
            temp = temp % 10;

            if(arr[i] != 0) flag = false;
            if(i == 0) break;
        }
        res += std::to_string(temp);
    }
    std::reverse(res.begin(), res.end());

    return res;
}

int compare_magnitude(const std::vector<uint32_t>& a, const std::vector<uint32_t>& b) {
    if(a.size() > b.size())
        return 1;
    if(a.size() < b.size())
        return -1;

    for(size_t i = a.size() - 1; i >= 0; i--) {
        if(a[i] > b[i])
            return 1;
        if(a[i] < b[i])
            return -1;
        if(i == 0) break;
    }
    return 0;
}

void add_magnitude(std::vector<uint32_t>& result, const std::vector<uint32_t>& a, const std::vector<uint32_t>& b) {
    size_t min_size = std::min(a.size(), b.size());
    size_t max_size = std::max(a.size(), b.size());
    if(&result == &a || &result == &b) {
        std::vector<uint32_t> temp(max_size + 1, 0);
        size_t index = 0;
        uint64_t base = 1ULL << 32;
        for(index = 0; index < max_size; index++) {
            uint64_t tmp = temp[index];
            if(index < a.size()) tmp += static_cast<uint64_t>(a[index]);
            if(index < b.size()) tmp += static_cast<uint64_t>(b[index]);
            temp[index] = tmp % base;
            temp[index + 1] = tmp / base;
        }

        trim(temp);
        result = std::move(temp);
        return;
    }

    size_t index = 0;
    uint64_t base = 1ULL << 32;
    result.assign(max_size + 1, 0);

    for(index = 0; index < max_size; index++) {
        uint64_t tmp = result[index];
        if(index < a.size()) tmp += static_cast<uint64_t>(a[index]);
        if(index < b.size()) tmp += static_cast<uint64_t>(b[index]);
        result[index] = tmp % base;
        result[index + 1] = tmp / base;
    }

    trim(result);
    return;
}

int main() {
    std::string a, b;
    std::cin>>a>>b;

    std::vector<uint32_t> n1 = decimalStr_to_uint32arr(a);
    std::vector<uint32_t> n2 = decimalStr_to_uint32arr(b);

    assert(uint32arr_to_decimalStr(n1) == a && uint32arr_to_decimalStr(n2) == b);

    std::cout<<compare_magnitude(n1, n2)<<std::endl;
    std::vector<uint32_t> res;
    add_magnitude(n1, n1, n2);
    std::cout<<uint32arr_to_decimalStr(n1)<<std::endl;
}