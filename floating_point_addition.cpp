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
#define STICKY_BIT 1
#define BIAS 127

#define DEBUG 0

enum NumberType { Zero, Normal, Denormal, Inf, NaN };

float bin_to_float(ulong x) {
    union {
        ulong  x;
        float  f;
    } num;
    num.x = x;
    return num.f;
}

ulong float_to_bin(float f) {
    union {
        ulong  x;
        float  f;
    } num;
    num.f = f;
    return num.x;
}

NumberType get_num_type(bitset<EXPONENT_BIT> num_exponent, bitset<FRACTION_BIT> num_fraction) {
    NumberType num_type;
    if (num_exponent.to_ulong() == 0) {
        if (num_fraction.to_ulong() == 0) {
            // number is zero
            num_type = Zero;
        }
        else {
            // number is denormal
            num_type = Denormal;
        }
    }
    else if (num_exponent.to_ulong() == 255) {
        if (num_fraction.to_ulong() == 0) {
            // number is +inf or -inf
            num_type = Inf;
        }
        else {
            // NaN
            num_type = NaN;
        }
    }
    else {
        // Normalized number
        num_type = Normal;
    }

    return num_type;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cout << "Enter all arguments" << endl;
        exit(EXIT_SUCCESS);
    }

    ifstream infile(argv[1]);
    ofstream outfile(argv[2]);
    string num1, num2;
    int instruction_count = 1;

    while (infile >> num1 >> num2)
    {
        std::cout << "------------------------------------" << endl;
        cout << "Test case #";
        cout << instruction_count++ << endl;
        cout << "Number 1: " << num1 << ", Number 2: " << num2 << endl;

        bitset<SIGN_BIT> num1_sign(num1, 0, SIGN_BIT);
        bitset<EXPONENT_BIT> num1_exponent(num1, SIGN_BIT, EXPONENT_BIT);
        bitset<FRACTION_BIT> num1_fraction(num1, SIGN_BIT + EXPONENT_BIT, FRACTION_BIT);
        bitset<SIGN_BIT+EXPONENT_BIT+FRACTION_BIT> num1_bitset(num1, 0, SIGN_BIT+EXPONENT_BIT+FRACTION_BIT);

        bitset<SIGN_BIT> num2_sign(num2, 0, SIGN_BIT);
        bitset<EXPONENT_BIT> num2_exponent(num2, SIGN_BIT, EXPONENT_BIT);
        bitset<FRACTION_BIT> num2_fraction(num2, SIGN_BIT + EXPONENT_BIT, FRACTION_BIT);
        bitset<SIGN_BIT+EXPONENT_BIT+FRACTION_BIT> num2_bitset(num2, 0, SIGN_BIT+EXPONENT_BIT+FRACTION_BIT);

        float resf = bin_to_float(num1_bitset.to_ulong()) + bin_to_float(num2_bitset.to_ulong());
        bitset<SIGN_BIT+EXPONENT_BIT+FRACTION_BIT> resb(float_to_bin(resf));
        cout << "Result computed by C++ = " << resf << "  " << resb.to_string() << endl;

        bool opposite_signs = num1_sign.to_ulong() != num2_sign.to_ulong() ? true : false;

        /*
            significand =
                <1.><fraction bit><guard bit><round bit><sticky bit>
                 1          23         1           1       1
        */

        bitset<1+FRACTION_BIT+GUARD_BIT+ROUND_BIT+STICKY_BIT> num1_significand;
        bitset<1+FRACTION_BIT+GUARD_BIT+ROUND_BIT+STICKY_BIT> num2_significand;

        bitset<SIGN_BIT> result_sign;
        bitset<2+FRACTION_BIT+GUARD_BIT+ROUND_BIT+STICKY_BIT> result_significand; // One exta bit for carry
        long result_exponent;
        string result;
        bool found_result = false;

        // number types
        NumberType num1_type = get_num_type(num1_exponent, num1_fraction);
        NumberType num2_type = get_num_type(num2_exponent, num2_fraction);
        #if DEBUG
            cout << "Number types= num1:  " << num1_type << " ,num2 = " << num2_type << endl;
        #endif

        if (num1_type == NaN || num2_type == NaN) {
            result = "nan";
            found_result = true;
        }
        else if (num1_type == Inf || num2_type == Inf) {
            if (opposite_signs && num1_type == Inf && num2_type == Inf) {
                result = "nan";
            }
            else {
                if (num1_sign.to_ulong() == 1) {
                    result = "-inf";
                }
                else {
                    result = "inf";
                }
            }
            found_result = true;
        }
        else if (num1_type == Zero && num2_type != Zero) {
            cout << "zero input" << endl;
            result = num2_bitset.to_string();
            found_result = true;
        }
        else if (num1_type != Zero && num2_type == Zero) {
            cout << "zero input" << endl;
            result = num1_bitset.to_string();
            found_result = true;
        }
        else if (num1_type == Zero && num2_type == Zero) {
            cout << "zero input" << endl;
            result = num1_bitset.to_string();
            found_result = true;
        }
        else if (num1_type == Denormal && num2_type == Denormal) {
            #if DEBUG
                cout << "Num 1 & 2 are denormal" << endl;
            #endif
            num1_exponent = bitset<EXPONENT_BIT>(1); // Actual exponent is -126
            num2_exponent = bitset<EXPONENT_BIT>(1); // Actual exponent is -126
            num1_significand = bitset<1+FRACTION_BIT+GUARD_BIT+ROUND_BIT+STICKY_BIT>("0" + num1_fraction.to_string() + "000");
            num2_significand = bitset<1+FRACTION_BIT+GUARD_BIT+ROUND_BIT+STICKY_BIT>("0" + num2_fraction.to_string() + "000");
        }
        else if (num1_type == Denormal && num2_type == Normal) {
            #if DEBUG
                cout << "Num 1 is denormal" << endl;
            #endif
            num1_exponent = bitset<EXPONENT_BIT>(1); // Actual exponent is -126
            num1_significand = bitset<1+FRACTION_BIT+GUARD_BIT+ROUND_BIT+STICKY_BIT>("0" + num1_fraction.to_string() + "000");
            num2_significand = bitset<1+FRACTION_BIT+GUARD_BIT+ROUND_BIT+STICKY_BIT>("1" + num2_fraction.to_string() + "000");
        }
        else if (num1_type == Normal && num2_type == Denormal) {
            #if DEBUG
                cout << "Num 2 is denormal" << endl;
            #endif
            num2_exponent = bitset<EXPONENT_BIT>(1); // Actual exponent is -126
            num1_significand = bitset<1+FRACTION_BIT+GUARD_BIT+ROUND_BIT+STICKY_BIT>("1" + num1_fraction.to_string() + "000");
            num2_significand = bitset<1+FRACTION_BIT+GUARD_BIT+ROUND_BIT+STICKY_BIT>("0" + num2_fraction.to_string() + "000");
        }
        else
        { // Both num types are normal
            num1_significand = bitset<1+FRACTION_BIT+GUARD_BIT+ROUND_BIT+STICKY_BIT>("1" + num1_fraction.to_string() + "000");
            num2_significand = bitset<1+FRACTION_BIT+GUARD_BIT+ROUND_BIT+STICKY_BIT>("1" + num2_fraction.to_string() + "000");
        }
        
        #if DEBUG
            cout << "\nNumbers initially" << endl;
            cout << "Numer 1:" << endl;
            cout << "   sign = " << num1_sign << endl;
            cout << "   exponent = " << num1_exponent << " " << num1_exponent.to_ulong() << endl;
            cout << "   fraction = " << num1_fraction << endl;
            cout << "   significand = " << num1_significand << endl;

            cout << "Numer 2:" << endl;
            cout << "   sign = " << num2_sign << endl;
            cout << "   exponent = " << num2_exponent << " " << num2_exponent.to_ulong() << endl;
            cout << "   fraction = " << num2_fraction << endl;
            cout << "   significand = " << num2_significand << endl;
        #endif

        // 1. Compare exponents
        int clock_cycles = 1;

        if (!found_result) {
            int diff = 0;
            int sticky = 0;
            if (num1_exponent.to_ulong() > num2_exponent.to_ulong()) {
                diff = num1_exponent.to_ulong() - num2_exponent.to_ulong();

                for (int i = 0; i < diff; i++)
                {
                    sticky = sticky | num2_significand[i+STICKY_BIT];
                }

                if (diff > 1+FRACTION_BIT+GUARD_BIT+ROUND_BIT) {
                    num2_significand = bitset<1+FRACTION_BIT+GUARD_BIT+ROUND_BIT+STICKY_BIT>(0);
                }
                else {
                    num2_significand = bitset<1+FRACTION_BIT+GUARD_BIT+ROUND_BIT+STICKY_BIT>(num2_significand.to_ulong() >> diff);
                }

                string stick_str = sticky == 0 ? "0" : "1";
                num2_significand = bitset<1+FRACTION_BIT+GUARD_BIT+ROUND_BIT+STICKY_BIT>(num2_significand.to_string().erase(FRACTION_BIT+GUARD_BIT+ROUND_BIT+STICKY_BIT) + stick_str);
                result_exponent = num1_exponent.to_ulong();
            }
            else {
                diff = num2_exponent.to_ulong() - num1_exponent.to_ulong();

                for (int i = 0; i < diff; i++)
                {
                    sticky = sticky | num1_significand[i+STICKY_BIT];
                }

                if (diff > 1+FRACTION_BIT+GUARD_BIT+ROUND_BIT) {
                    num1_significand = bitset<1+FRACTION_BIT+GUARD_BIT+ROUND_BIT+STICKY_BIT>(0);
                }
                else {
                    num1_significand = bitset<1+FRACTION_BIT+GUARD_BIT+ROUND_BIT+STICKY_BIT>(num1_significand.to_ulong() >> diff);
                }

                string stick_str = sticky == 0 ? "0" : "1";
                num1_significand = bitset<1+FRACTION_BIT+GUARD_BIT+ROUND_BIT+STICKY_BIT>(num1_significand.to_string().erase(FRACTION_BIT+GUARD_BIT+ROUND_BIT+STICKY_BIT) + stick_str);
                result_exponent = num2_exponent.to_ulong();
            }

            #if DEBUG
                cout << "\nAfter STEP 1: compare exponents" << endl;
                cout << "exponent diff = " << diff << " , sticky = " << sticky << endl;

                cout << "Numer 1:" << endl;
                cout << "   sign = " << num1_sign << endl;
                cout << "   exponent = " << num1_exponent << " " << num1_exponent.to_ulong() << endl;
                cout << "   fraction = " << num1_fraction << endl;
                cout << "   significand = " << num1_significand << endl;

                cout << "Numer 2:" << endl;
                cout << "   sign = " << num2_sign << endl;
                cout << "   exponent = " << num2_exponent << " " << num2_exponent.to_ulong() << endl;
                cout << "   fraction = " << num2_fraction << endl;
                cout << "   significand = " << num2_significand << endl;
            #endif


            // 2. Add significands
            clock_cycles += 1;
            if (!opposite_signs) {
                result_sign = num1_sign;
                result_significand = bitset<2+FRACTION_BIT+GUARD_BIT+ROUND_BIT+STICKY_BIT>(num1_significand.to_ulong() + num2_significand.to_ulong());
            }
            else {
                if (num1_significand.to_ulong() > num2_significand.to_ulong()) {
                    result_sign = num1_sign;
                    result_significand = bitset<2+FRACTION_BIT+GUARD_BIT+ROUND_BIT+STICKY_BIT>(num1_significand.to_ulong() - num2_significand.to_ulong());
                }
                else {
                    result_sign = num2_sign;
                    result_significand = bitset<2+FRACTION_BIT+GUARD_BIT+ROUND_BIT+STICKY_BIT>(num2_significand.to_ulong() - num1_significand.to_ulong());
                }
            }

            #if DEBUG
                cout << "\nAfter STEP 2: Add significands" << endl;
                cout << "Result:" << endl;
                cout << "   sign = " << result_sign << endl;
                cout << "   exponent = " << result_exponent << endl;
                cout << "   significand = " << result_significand << endl;
            #endif

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
                else if (result_significand[1+FRACTION_BIT+GUARD_BIT+ROUND_BIT+STICKY_BIT] == 1) {
                    // right shift
                    #if DEBUG
                        cout << " shift right" << endl;
                    #endif
                    int new_sticky = result_significand[STICKY_BIT-1] | result_significand[STICKY_BIT];
                    result_significand = result_significand >> 1;
                    result_significand[STICKY_BIT-1] = new_sticky;
                    result_exponent += 1;
                }
                else {
                    if (result_significand[FRACTION_BIT+GUARD_BIT+ROUND_BIT+STICKY_BIT] == 1) {
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
                        while (result_significand[FRACTION_BIT+GUARD_BIT+ROUND_BIT+STICKY_BIT] == 0)
                        {
                            result_significand = result_significand << 1;
                            left_shifts++;
                        }
                        result_exponent -= left_shifts;
                    }
                }

                #if DEBUG
                    cout << "\nAfter STEP 3: Normalize" << endl;
                    cout << "Result:" << endl;
                    cout << "   sign = " << result_sign << endl;
                    cout << "   exponent = " << result_exponent << endl;
                    cout << "   significand = " << result_significand << endl;
                #endif

                if (result_exponent - BIAS < -126) {
                    // std::cout << "underflow" << endl;
                    underflow = true;
                    break;
                }

                if (result_exponent - BIAS > 127) {
                    // std::cout << "overflow" << endl;
                    overflow = true;
                    break;
                }

                // 4. Rounding
                clock_cycles += 1;
                bool round_up = false;
                int guard_bit_value = result_significand[GUARD_BIT+ROUND_BIT+STICKY_BIT-1];
                int round_bit_value = result_significand[ROUND_BIT+STICKY_BIT-1];
                int sticky_bit_value = result_significand[STICKY_BIT-1];

                #if DEBUG
                    cout << endl;
                    cout << "STEP 4: Rounding" << endl;
                    cout << "GRS = " << guard_bit_value << " " << round_bit_value << " " << sticky_bit_value << endl;
                #endif

                if (guard_bit_value == 0) {
                    // 0xx = nothing
                    round_up = false;
                }
                else {
                    if (round_bit_value == 0 && sticky_bit_value == 0) {
                        // tie
                        #if DEBUG
                            cout << "Tie breaker case" << endl;
                        #endif
                        if (result_significand[1+GUARD_BIT+ROUND_BIT+STICKY_BIT-1] == 1) {
                            // round up
                            round_up = true;
                        }
                        else {
                            // nothing
                            round_up = false;
                        }
                    }
                    else {
                        // other cases = round up
                        round_up = true;
                    }
                }

                if (round_up) {
                    std::cout << "Rounding up..." << endl;
                    sticky = 0;
                    bitset<2+FRACTION_BIT+GUARD_BIT+ROUND_BIT+STICKY_BIT> one_bit("1000");

                    result_significand = bitset<2+FRACTION_BIT+GUARD_BIT+ROUND_BIT+STICKY_BIT>(result_significand.to_ulong() + one_bit.to_ulong());
                }

                if (result_significand.to_ulong() == 0) {
                    still_normalized = true;
                }
                else if (result_significand[1+FRACTION_BIT+GUARD_BIT+ROUND_BIT+STICKY_BIT] == 1) {
                    still_normalized = false;
                }
                else {
                    if (result_significand[FRACTION_BIT+GUARD_BIT+ROUND_BIT+STICKY_BIT] == 1) {
                        still_normalized = true;
                    }
                    else {
                        still_normalized = false;
                    }
                }

                #if DEBUG
                    cout << "\nAfter STEP 4: Rounding" << endl;
                    cout << "Result:" << endl;
                    cout << "   sign = " << result_sign << endl;
                    cout << "   exponent = " << result_exponent << endl;
                    cout << "   significand = " << result_significand << endl;
                    std::cout << "Still normalized = " << still_normalized << endl;
                #endif

            } while (!still_normalized);

            if (underflow) {
                result = "underflow";
            }
            else if (overflow) {
                result = "overflow";
            }
            else if (zero) {
                std::cout << "Zero" << endl;
                bitset<SIGN_BIT+EXPONENT_BIT+FRACTION_BIT> zero_bits(0);
                result = zero_bits.to_string();
            }
            else {
                #if DEBUG
                    cout << "Res significand = " << result_significand.to_string() << endl;
                #endif
                string result_fraction = result_significand.to_string().erase(2+FRACTION_BIT).erase(0,2);
                result = result_sign.to_string() + bitset<EXPONENT_BIT>(result_exponent).to_string() + result_fraction;

                if (result != resb.to_string()) {
                    cout << "(Oops! Didn't match with C++'s adder.)" << endl;
                }
                else {
                    cout << "(Matched with C++'s adder!)" << endl;
                }
            }
        }

        std::cout << "Result = " << result << endl;
        std::cout << "Clock cyles = " << clock_cycles << endl;
        outfile << result << " " << clock_cycles << endl;
    }
    outfile.close();
}