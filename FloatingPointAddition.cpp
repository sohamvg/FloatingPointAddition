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

#define DEBUG 0

int main() {
    ifstream infile("inp.txt");
    string num1, num2;

    while (infile >> num1 >> num2)
    {
        std::cout << "------------------------------------" << endl;

        bitset<SIGN_BIT> num1_sign(num1, 0, SIGN_BIT);
        bitset<EXPONENT_BIT> num1_exponent(num1, SIGN_BIT, EXPONENT_BIT);
        bitset<FRACTION_BIT> num1_fraction(num1, SIGN_BIT + EXPONENT_BIT, FRACTION_BIT);

        bitset<SIGN_BIT> num2_sign(num2, 0, SIGN_BIT);
        bitset<EXPONENT_BIT> num2_exponent(num2, SIGN_BIT, EXPONENT_BIT);
        bitset<FRACTION_BIT> num2_fraction(num2, SIGN_BIT + EXPONENT_BIT, FRACTION_BIT);

        /*
            significand =
                <1.><fraction bit><guard bit><round bit>
                 1          23              1           1
        */

        // First 1 bit for characterstic & last 2 bits are guard bit and round bit
        bitset<1+FRACTION_BIT+GUARD_BIT+ROUND_BIT> num1_significand("1" + num1_fraction.to_string() + "00");
        bitset<1+FRACTION_BIT+GUARD_BIT+ROUND_BIT> num2_significand("1" + num2_fraction.to_string() + "00");

        bool opposite_signs = num1_sign.to_ulong() != num2_sign.to_ulong() ? true : false;

        #if DEBUG
            cout << num1_sign << " " << num1_exponent  << " " << num1_fraction << " " << num1_significand << endl;
            cout << num1_sign.to_ulong() << " " << num1_exponent.to_ulong() << " " << num1_fraction.to_ulong() << endl;

            cout << num2_sign << " " << num2_exponent  << " " << num2_fraction << " " << num2_significand << endl;
            cout << num2_sign.to_ulong() << " " << num2_exponent.to_ulong() << " " << num2_fraction.to_ulong() << endl;
        #endif


        bitset<SIGN_BIT> result_sign;
        bitset<2+FRACTION_BIT+GUARD_BIT+ROUND_BIT> result_significand; // One exta bit for carry
        long result_exponent;

        // 1. Compare exponents
        int clock_cycles = 1;
        int diff = 0;
        int sticky = 0;
        if (num1_exponent.to_ulong() > num2_exponent.to_ulong()) {
            diff = num1_exponent.to_ulong() - num2_exponent.to_ulong();

            for (int i = 0; i < diff; i++)
            {
                sticky = sticky | num2_significand[i];
            }

            num2_significand = bitset<1+FRACTION_BIT+GUARD_BIT+ROUND_BIT>(num2_significand.to_ulong() >> diff);
            result_exponent = num1_exponent.to_ulong();
        }
        else {
            diff = num2_exponent.to_ulong() - num1_exponent.to_ulong();

            for (int i = 0; i < diff; i++)
            {
                sticky = sticky | num2_significand[i];
            }

            num1_significand = bitset<1+FRACTION_BIT+GUARD_BIT+ROUND_BIT>(num1_significand.to_ulong() >> diff);
            result_exponent = num2_exponent.to_ulong();
        }

        #if DEBUG
            cout << num1_sign << " " << num1_exponent  << " " << num1_fraction << " " << num1_significand << endl;
            cout << num1_sign.to_ulong() << " " << num1_exponent.to_ulong() << " " << num1_fraction.to_ulong() << endl;

            cout << num2_sign << " " << num2_exponent  << " " << num2_fraction << " " << num2_significand << endl;
            cout << num2_sign.to_ulong() << " " << num2_exponent.to_ulong() << " " << num2_fraction.to_ulong() << endl;
        #endif


        // 2. Add significands
        clock_cycles += 1;
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

        bool still_normalized = false;
        bool underflow = false;
        bool overflow = false;
        bool zero = false;
        do
        {
            // 3. Normalize
            clock_cycles += 1;
            if (result_significand.to_ulong() == 0) {
                // zero
                zero = true;
                break;
            }
            else if (result_significand[1+FRACTION_BIT+GUARD_BIT+ROUND_BIT] == 1) {
                // right shift
                #if DEBUG
                    cout << " shift right" << endl;
                #endif
                result_significand = result_significand >> 1;
                result_exponent += 1;
            }
            else {
                if (result_significand[FRACTION_BIT+GUARD_BIT+ROUND_BIT] == 1) {
                    // Already normalized
                    #if DEBUG
                        cout << "already normalized" << endl;
                    #endif
                }
                else {
                    // left shift
                    #if DEBUG
                        cout << "shift left" << endl;
                    #endif
                    int left_shifts = 0;
                    while (result_significand[FRACTION_BIT+GUARD_BIT+ROUND_BIT] == 0)
                    {
                        result_significand = result_significand << 1;
                        left_shifts++;
                    }
                    result_exponent -= left_shifts;
                }
            }

            if (result_exponent - BIAS < -126) {
                std::cout << "underflow" << endl;
                underflow = true;
                break;
            }

            if (result_exponent - BIAS > 127) {
                std::cout << "overflow" << endl;
                overflow = true;
                break;
            }

            // 4. Rounding
            clock_cycles += 1;
            bool round_up = false;
            int guard_bit_value = result_significand[GUARD_BIT+ROUND_BIT-1];
            int round_bit_value = result_significand[ROUND_BIT-1];

            if (guard_bit_value == 0) {
                // 0xx = nothing
            }
            else {
                if (round_bit_value == 0 && sticky == 0) {
                    // tie
                    if (result_significand[GUARD_BIT+ROUND_BIT] == 1) {
                        // round up
                        round_up = true;
                    }
                    else {
                        // nothing
                    }
                }
                else {
                    // other cases = round up
                    round_up = true;
                }
            }

            if (round_up) {
                sticky = 0;
                result_significand[ROUND_BIT-1] = 0;
                result_significand[GUARD_BIT+ROUND_BIT-1] = 0;
                bitset<2+FRACTION_BIT+GUARD_BIT+ROUND_BIT> one_bit("100");

                result_significand = bitset<2+FRACTION_BIT+GUARD_BIT+ROUND_BIT>(result_significand.to_ulong() + one_bit.to_ulong());
            }

            if (result_significand.to_ulong() == 0) {
                still_normalized = true;
            }
            else if (result_significand[1+FRACTION_BIT+GUARD_BIT+ROUND_BIT] == 1) {
                still_normalized = false;
            }
            else {
                if (result_significand[FRACTION_BIT+GUARD_BIT+ROUND_BIT] == 1) {
                    still_normalized = true;
                }
                else {
                    still_normalized = false;
                }
            }

        } while (!still_normalized);

        if (underflow) {
            std::cout << "Underflow occured" << endl;
        }
        else if (overflow) {
            std::cout << "Overflow occured" << endl;
        }
        else if (zero) {
            std::cout << "Zero" << endl;
            bitset<SIGN_BIT+EXPONENT_BIT+FRACTION_BIT> zero_bits(0);
            std::cout << "Result = " << zero_bits.to_string() << endl;
        }
        else {
            std::cout << "result " << result_sign << " " << result_significand << " " << result_exponent - BIAS << endl;

            string result_fraction = result_significand.to_string().erase(FRACTION_BIT+GUARD_BIT+ROUND_BIT, GUARD_BIT+ROUND_BIT).erase(0,2);
            string result = result_sign.to_string() + bitset<EXPONENT_BIT>(result_exponent).to_string() + result_fraction;

            std::cout << "Result = " << result << endl;
        }
        std::cout << "Clock cycles = " << clock_cycles << endl;
    }
}