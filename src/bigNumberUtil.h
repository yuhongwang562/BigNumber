#ifndef BIGNUMBERUTIL_H
#define BIGNUMBERUTIL_H

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <exception>

/*
    將合法字串轉為 uint32_t 陣列儲存，
    輸入: 合法字串
    輸出: 最後一項非零的 uint32_t 陣列

    合法字串為: 1. 無前導零 2. 字串除該進制下的數字外，無其他字元 3. 非空字串
    保證一個數值只有一種陣列形式
*/
std::vector<uint32_t> binaryStr_to_uint32arr(const std::string& binaryStr);
std::vector<uint32_t> decimalStr_to_uint32arr(const std::string& decimalStr);
std::vector<uint32_t> HexStr_to_uint32arr(const std::string& hexStr);

/*
    將 uint32_t 陣列轉為對應 base 進制下的字串
    保證 arr 為最後一項非零的 uint32_t 陣列
*/
std::string uint32arr_to_BaseStr(std::vector<uint32_t> arr, int base = 10);

/*
    比較 a b 大小或是否相等
    回傳值: 1(a > b), 0(a == b), -1(a < b)
*/
int compare_magnitude(const std::vector<uint32_t>& a, const std::vector<uint32_t>& b);

/*
    將 a + b 付值至 result
    須保證 a, b 皆為最後一項非零的 uint32_t
*/
void add_magnitude(std::vector<uint32_t>& result, const std::vector<uint32_t>& a, const std::vector<uint32_t>& b);

/*
    將 a - b 付值至 result
    須保證 a, b 皆為最後一項非零的 uint32_t 且 a >= b
*/
void sub_magnitude(std::vector<uint32_t>& result, const std::vector<uint32_t>& a, const std::vector<uint32_t>& b);

#endif