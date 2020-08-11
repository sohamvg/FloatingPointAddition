#include <iostream>
#include <fstream>
#include <string>
#include <bitset>

using namespace std;

#define SIGN_BIT 1
#define EXPONENT_BIT 8
#define FRACTION_BIT 23
#define GUARD_BIT 1
#define ROUND_BIT 1
#define BIAS 127

int getSign(bitset<SIGN_BIT> b) {
    if (b.to_ulong() == 0) {
        return 1;
    }
    else return -1;
}

int main() {
    ifstream infile("inp.txt");
    string num1, num2;

    while (infile >> num1 >> num2)
    {
        cout << "------------------------------------" << endl;

        bitset<SIGN_BIT> num1_sign(num1, 0, SIGN_BIT);
        bitset<EXPONENT_BIT> num1_exponent(num1, SIGN_BIT, EXPONENT_BIT);
        bitset<FRACTION_BIT> num1_fraction(num1, SIGN_BIT + EXPONENT_BIT, FRACTION_BIT);

        bitset<SIGN_BIT> num2_sign(num2, 0, SIGN_BIT);
        bitset<EXPONENT_BIT> num2_exponent(num2, SIGN_BIT, EXPONENT_BIT);
        bitset<FRACTION_BIT> num2_fraction(num2, SIGN_BIT + EXPONENT_BIT, FRACTION_BIT);

        /*
            significand = 
                <1.><fraction bits><guard bits><round bits>
                 1          23              1           1
        */

        // First 1 bit for characterstic
        bitset<1+FRACTION_BIT+GUARD_BIT+ROUND_BIT> num1_significand("1" + num1_fraction.to_string());
        bitset<1+FRACTION_BIT+GUARD_BIT+ROUND_BIT> num2_significand("1" + num2_fraction.to_string());

        bool opposite_signs = num1_sign.to_ulong() != num2_sign.to_ulong() ? true : false;

        cout << num1_sign << " " << num1_exponent  << " " << num1_fraction << " " << num1_significand << endl;
        cout << num1_sign.to_ulong() << " " << num1_exponent.to_ulong() << " " << num1_fraction.to_ulong() << endl;

        cout << num2_sign << " " << num2_exponent  << " " << num2_fraction << " " << num2_significand << endl;
        cout << num2_sign.to_ulong() << " " << num2_exponent.to_ulong() << " " << num2_fraction.to_ulong() << endl;

        bitset<SIGN_BIT> result_sign;
        bitset<2+FRACTION_BIT+GUARD_BIT+ROUND_BIT> result_significand; // One exta bit for carry
        bitset<EXPONENT_BIT> result_exponent;

        int diff = 0;
        if (num1_exponent.to_ulong() > num2_exponent.to_ulong()) {
            diff = num1_exponent.to_ulong() - num2_exponent.to_ulong();
            num2_significand = bitset<1+FRACTION_BIT+GUARD_BIT+ROUND_BIT>(num2_significand.to_ulong() >> diff);
            result_exponent = num1_exponent;
        }
        else {
            diff = num2_exponent.to_ulong() - num1_exponent.to_ulong();
            num1_significand = bitset<1+FRACTION_BIT+GUARD_BIT+ROUND_BIT>(num2_significand.to_ulong() >> diff);
            result_exponent = num2_exponent;
        }

        cout << num1_sign << " " << num1_exponent  << " " << num1_fraction << " " << num1_significand << endl;
        cout << num1_sign.to_ulong() << " " << num1_exponent.to_ulong() << " " << num1_fraction.to_ulong() << endl;

        cout << num2_sign << " " << num2_exponent  << " " << num2_fraction << " " << num2_significand << endl;
        cout << num2_sign.to_ulong() << " " << num2_exponent.to_ulong() << " " << num2_fraction.to_ulong() << endl;


        if (!opposite_signs) {
            result_sign = num1_sign;
            result_significand = bitset<2+FRACTION_BIT+GUARD_BIT+ROUND_BIT>(num1_significand.to_ulong() + num2_significand.to_ulong());
        }
        else {
            if (num1_significand.to_ulong() > num2_significand.to_ulong()) {
                result_sign = num1_sign;
                result_significand = bitset<2+FRACTION_BIT+GUARD_BIT+ROUND_BIT>(num1_significand.to_ulong() - num2_significand.to_ulong());
            }
            else {
                result_sign = num2_sign;
                result_significand = bitset<2+FRACTION_BIT+GUARD_BIT+ROUND_BIT>(num2_significand.to_ulong() - num1_significand.to_ulong());
            }
        }
        
        cout << result_sign << " " << result_significand << " " << result_exponent.to_ulong() << endl;

        // Normalize
        if (result_significand.to_ulong() == 0) {
            // zero
        }
        else if (result_significand[1+FRACTION_BIT] == 1) {
            // right shift
            cout << "rifht " << endl;
            result_significand = result_significand >> 1;
            result_exponent = bitset<EXPONENT_BIT>(result_exponent.to_ulong() + 1);
        }
        else {
            if (result_significand[FRACTION_BIT] == 1) {
                // Already normalized
                cout << "already" << endl;
            }
            else {
                cout << "left" << endl;
                // left shift
                int left_shifts = 0;
                while (result_significand[FRACTION_BIT] == 0 && left_shifts <= FRACTION_BIT + 1)
                {
                    result_significand = result_significand << 1;
                    left_shifts++;
                }
                result_exponent = bitset<EXPONENT_BIT>(result_exponent.to_ulong() - left_shifts);
            }
        }

        cout << "result " << result_sign << " " << result_significand << " " << result_exponent.to_ulong() << endl;

        if (result_exponent.to_ulong() - BIAS < -126) {
            cout << "underflow" << endl;
            continue;
        }

        if (result_exponent.to_ulong() - BIAS > 127) {
            cout << "overflow" << endl;
            continue;
        }
    }
}