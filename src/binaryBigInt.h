#ifndef BINARYBIGINT_H
#define BINARYBIGINT_H

#include <cstdint>
#include <iostream>
#include <ostream>
#include <string>
#include <vector>

class BinaryBigInt {
private:
    // sign = true 為正號，false 為負號
    bool sign;
    std::vector<uint32_t> digits;

    void trim();
    void consturct_by_binaryStr(const std::string& binaryStr);
    void consturct_by_decimalStr(const std::string& decimalStr);

public:
    BinaryBigInt();
    BinaryBigInt(const std::string& numberStr);
    BinaryBigInt(const char* c_str);
    ~BinaryBigInt() = default;

    uint64_t bit_length() const;
    uint64_t limb() const;
    bool is_zero() const;

    BinaryBigInt operator+(const BinaryBigInt& other);
    BinaryBigInt operator-(const BinaryBigInt& other);
    BinaryBigInt operator*(const BinaryBigInt& other);
    BinaryBigInt operator/(const BinaryBigInt& other);

    bool operator>(const BinaryBigInt& other);
    bool operator<(const BinaryBigInt& other);
    bool operator==(const BinaryBigInt& other);
    bool operator!=(const BinaryBigInt& other);
    bool operator>=(const BinaryBigInt& other);
    bool operator<=(const BinaryBigInt& other);

    friend std::ostream& operator<<(std::ostream& os, const BinaryBigInt& bigInt);
    friend std::string to_string(const BinaryBigInt& bigInt);
};

std::ostream& operator<<(std::ostream& os, const BinaryBigInt& bigInt);
std::string to_string(const BinaryBigInt& bigInt);

#endif