#include "./binaryBigInt.h"
#include "./bigNumberUtil.h"
#include <algorithm>
#include <cstdint>
#include <ostream>
#include <string>

void BinaryBigInt::trim() {
    while(digits.size() > 1 && digits.back() == 0) {
        digits.pop_back();
    }
    if(digits.empty()) {
        digits.push_back(0);
    }
}

void BinaryBigInt::consturct_by_binaryStr(const std::string& binaryStr) {
    sign = true;
    digits = binaryStr_to_uint32arr(binaryStr);
    trim();
}

void BinaryBigInt::consturct_by_decimalStr(const std::string& decimalStr) {
    if(decimalStr[0] == '-')
        sign = false;
    else
        sign = true;

    if(decimalStr[0] == '-' || decimalStr[0] == '+')
        digits = decimalStr_to_uint32arr(decimalStr.substr(1));
    else
        digits = decimalStr_to_uint32arr(decimalStr);
    trim();
}


BinaryBigInt::BinaryBigInt(): sign(true), digits(1, 0) {}

bool is_binaryStr(const std::string& numberStr) {
    if(numberStr[0] == '0' && numberStr[1] == 'x')
        return true;
    return false;
}

BinaryBigInt::BinaryBigInt(const std::string& numberStr) {
    if(is_binaryStr(numberStr)) {
        BinaryBigInt::consturct_by_binaryStr(numberStr.substr(2));
    }
    else
        BinaryBigInt::consturct_by_decimalStr(numberStr);
}

BinaryBigInt::BinaryBigInt(const char* c_str): BinaryBigInt(std::string(c_str)) {}

uint64_t BinaryBigInt::bit_length() const {
    if(is_zero())
        return 0;

    uint32_t last = digits.back();
    
    uint32_t num = 0;
    while(last < (1 << 31)) {
        last <<= 1;
        num++;
    }

    return static_cast<uint64_t>(32 * digits.size() - num);
}

uint64_t BinaryBigInt::limb() const {
    return digits.size();
}

bool BinaryBigInt::is_zero() const {
    if(digits.size() == 1 && digits[0] == 0)
        return true;
    return false;
}

bool BinaryBigInt::is_negative() const {
    return !sign;
}

BinaryBigInt BinaryBigInt::operator+(const BinaryBigInt& other) {    
    
}

bool BinaryBigInt::operator>(const BinaryBigInt& other) {
    // 正數 > 負數 和 負數 < 正數 (0 的 sigin 皆為正)
    if(sign == true && other.sign == false)
        return true;
    if(sign == false && other.sign == true)
        return false;

    uint32_t bit_len = bit_length();
    uint32_t obit_len = other.bit_length();
    
    // 處理同號時，正負號各自情況
    if(bit_len > obit_len) {
        if(sign == true) return true;
        else return false;
    }
    if(bit_len < obit_len) {
        if(sign == true) return false;
        else return true;
    }

    bool is_equal = true;
    
    // 若 i 為 uint32_t ， i = 0, i-- -> i = 2^32 - 1 and i > digit.size()
    // 所以 i 改為 int64_t;
    for(int64_t i = static_cast<int64_t>(digits.size() - 1); i >= 0; i--) {
        // 處理同號時，正負號各自情況
        if(digits[i] < other.digits[i]) {
            if(sign == true) return false;
            else return true;
        }

        if(is_equal == true && digits[i] != other.digits[i])
            is_equal = false;
    }
    if(is_equal) return false;

    if(sign == true)
        return true;
    return false;
}

bool BinaryBigInt::operator<(const BinaryBigInt& other) {
    return !(*this > other || *this == other);
}

bool BinaryBigInt::operator==(const BinaryBigInt& other) {
    if(sign != other.sign)
        return false;

    uint32_t bit_len = bit_length();
    uint32_t obit_len = other.bit_length();
    if(bit_len != obit_len)
        return false;

    for(uint32_t i = 0; i < digits.size(); i++) {
        if(digits[i] != other.digits[i])
            return false;
    }

    return true;
}

bool BinaryBigInt::operator!=(const BinaryBigInt& other) {
    return !(*this == other);
}

bool BinaryBigInt::operator>=(const BinaryBigInt& other) {
    return (*this > other || *this == other);
}

bool BinaryBigInt::operator<=(const BinaryBigInt& other) {
    return (*this < other || *this == other);
}

std::ostream& operator<<(std::ostream& os, const BinaryBigInt& bigInt) {
    if(bigInt.sign == false)
        os << "-";
    os << uint32arr_to_decimalStr(bigInt.digits);
    return os;
}

std::string to_string(const BinaryBigInt& bigInt) {
    std::string res = uint32arr_to_decimalStr(bigInt.digits);
    if(bigInt.sign == false)
        return "-" + res;
    return res;
}