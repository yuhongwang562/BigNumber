#include <atomic>
#include <exception>
#include <ios>
#include <iostream>
#include <string>
#include <cassert>
#include "./binaryBigInt.h"
using namespace std;

// TODO: 完成建構子測試
void test_construct() {
    // 默認建構子測試
    {
        BinaryBigInt num;
        assert(num.is_zero() == true);
        assert(num.is_negative() == false);
        assert(num.bit_length() == 0);
        assert(num.limb() == 1);
        assert(to_string(num) == "0");
    }
    
    {
        BinaryBigInt num = std::string("12345");
        assert(num.is_zero() == false);
        assert(num.is_negative() == false);
        assert(num.bit_length() == 14);
        assert(num.limb() == 1);
    }
    BinaryBigInt num2 = std::string("12345");
    assert(num2.bit_length() == 14);
    assert(num2.limb() == 1);
    assert(to_string(num2) == "12345");
    cout<<"通過 string 建構子測試"<<endl;

    // TODO: 測試 c 風格字串建構子
    BinaryBigInt num3 = "12345";
    assert(num3.bit_length() == 14);
    assert(num3.limb() == 1);
    assert(to_string(num3) == "12345");
    cout<<"通過 c string 建構子測試"<<endl;

    // TODO: 測試負數的資料儲存是否正確
    BinaryBigInt num4 = "-5";
    assert(num4.is_negative() == true);
    assert(num4.bit_length() == 3);
    assert(num4.limb() == 1);
    assert(to_string(num4) == "-5");
    cout<<"通過負數測試"<<endl;

    // TODO: 測試剛好等於 2^32 - 1 和 2^32 數值的資料儲存是否正確
    BinaryBigInt num5 = "4294967296"; // 2^32
    BinaryBigInt num6 = "4294967295"; // 2^32 - 1
    assert(num5.bit_length() == 33 && num6.bit_length() == 32);
    assert(num5.limb() == 2 && num6.limb() == 1);
    assert(to_string(num5) == "4294967296" && to_string(num6) == "4294967295");
    cout<<"通過 32 位元數字表示測試"<<endl;

    // TODO: 測試含前導零字串以及不含前導零字串的資料儲存是否正確
    BinaryBigInt num7 = std::string("00000001203");
    BinaryBigInt num8 = "1203";
    assert(num7.bit_length() == num8.bit_length() && num7.bit_length() == 11);
    assert(num7.limb() == num8.limb() && num7.limb() == 1);
    assert(to_string(num7) == to_string(num8) && to_string(num7) == "1203");
    cout<<"通過前導零字串驗證"<<endl;
}

// TODO: 完成比較運算子正確性測試
void test_compare() {
    BinaryBigInt a = "0";
    BinaryBigInt b = "123456";
    BinaryBigInt c = "000123456";
    BinaryBigInt d = "-11";
    BinaryBigInt e = "-22";
    BinaryBigInt f = "-11";
    BinaryBigInt g = "00000";

    cout<<boolalpha
        <<(a == b) << endl
        <<(a == g) << endl
        <<(a >= g) << endl
        <<(a <= g) << endl
        <<(a > g)  << endl
        <<(a < g)  <<endl
        <<(b >= c) << endl
        <<(b <= c) << endl
        <<(b == c) << endl
        <<(b > c)  << endl
        <<(b < c)  << endl
        <<(d > b)  << endl
        <<(d < b)  << endl
        <<(d == b) << endl
        <<(d >= b) << endl
        <<(d <= b) << endl
        ;
}

int main() {
    BinaryBigInt a = "12";
    BinaryBigInt b = a;
    b = "20";
    cout<<a<<" "<<b;
}