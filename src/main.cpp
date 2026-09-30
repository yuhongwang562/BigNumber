#include <iostream>
#include <string>
#include <cassert>
#include "./binaryBigInt.h"
using namespace std;

void test_construct() {
    BinaryBigInt num1;
    assert(num1.is_zero() == true);
    assert(num1.bit_length() == 0);
    assert(num1.limb() == 1);
    assert(to_string(num1) == "0");
    cout<<"通過默認建構子測試"<<endl;

    BinaryBigInt num2 = std::string("12345");
    assert(num2.bit_length() == 14);
    assert(num2.limb() == 1);
    assert(to_string(num2) == "12345");
    cout<<"通過 string 建構子測試"<<endl;
}

int main() {
    test_construct();
    cout<<"通過建構子測試"<<endl;
}