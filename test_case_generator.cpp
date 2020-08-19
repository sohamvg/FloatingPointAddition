#include <stdlib.h>
#include <iostream>
#include <bitset>
#include <math.h>
#include <vector>
#include <algorithm>
#include <fstream>

using namespace std;

#define SIGN_BIT 1
#define EXPONENT_BIT 8
#define FRACTION_BIT 23
#define MAX_FRACTION 8388607

#define RANDOM_FRACTIONS 20
#define RANDOM_EXPONENTS 5

int main()
{
    // Total test cases generated = RANDOM_FRACTIONS * (RANDOM_EXPONENTS + #custom exponents) * (RANDOM_EXPONENTS + #custom exponents) * 2 + #custom test cases.

    vector<int> exponents = {0, 1, 9, 10, 99, 100, 101, 126, 127, 128, 254, 255}; // custom exponents

    for (int i = 0; i < RANDOM_EXPONENTS; i++)
    {
        int num = rand() % (255 + 1);
        if (std::find(exponents.begin(), exponents.end(), num) == exponents.end()) {
            exponents.push_back(num); 
        }
    }
    
    ofstream inputfile, ansfile;
    inputfile.open("inp_large.txt");
    int count = 0;
    
    for (int i = 0; i < RANDOM_FRACTIONS; i++)
    {
        int fraction1 = rand() % (MAX_FRACTION + 1);
        int fraction2 = rand() % (MAX_FRACTION + 1);

        for (int exp1 = 0; exp1 < exponents.size(); exp1++)
        {
            for (int exp2 = 0; exp2 < exponents.size(); exp2++)
            {
                int exponent1 = exponents[exp1];
                int exponent2 = exponents[exp2];

                // cout << exp1 << " " << exponent1 << " " << exp2 << " " << exponent2 << endl;

                bitset<FRACTION_BIT> f1(fraction1);
                bitset<FRACTION_BIT> f2(fraction2);
                bitset<EXPONENT_BIT> e1(exponent1);
                bitset<EXPONENT_BIT> e2(exponent2);
                bitset<SIGN_BIT+EXPONENT_BIT+FRACTION_BIT> n1("0" + e1.to_string() + f1.to_string(), 0, SIGN_BIT+EXPONENT_BIT+FRACTION_BIT);
                bitset<SIGN_BIT+EXPONENT_BIT+FRACTION_BIT> n2("0" + e2.to_string() + f2.to_string(), 0, SIGN_BIT+EXPONENT_BIT+FRACTION_BIT);
                bitset<SIGN_BIT+EXPONENT_BIT+FRACTION_BIT> n3("1" + e2.to_string() + f2.to_string(), 0, SIGN_BIT+EXPONENT_BIT+FRACTION_BIT);
                inputfile << n1.to_string() << " " << n2.to_string() << endl;
                inputfile << n1.to_string() << " " << n3.to_string() << endl;

                count+=2;
            }
        }
    }

    // Custom testcases
    string fraction1 = "01010101010101010101010";
    string fraction2 = "01010101010101010101011";
    
    for (int exp1 = 0; exp1 < exponents.size(); exp1++)
    {
        for (int exp2 = 0; exp2 < exponents.size(); exp2++)
        {
            int exponent1 = exponents[exp1];
            int exponent2 = exponents[exp2];

            bitset<FRACTION_BIT> f1(fraction1, 0, FRACTION_BIT);
            bitset<FRACTION_BIT> f2(fraction2, 0, FRACTION_BIT);
            bitset<EXPONENT_BIT> e1(exponent1);
            bitset<EXPONENT_BIT> e2(exponent2);
            bitset<SIGN_BIT+EXPONENT_BIT+FRACTION_BIT> n1("0" + e1.to_string() + f1.to_string(), 0, SIGN_BIT+EXPONENT_BIT+FRACTION_BIT);
            bitset<SIGN_BIT+EXPONENT_BIT+FRACTION_BIT> n2("0" + e2.to_string() + f2.to_string(), 0, SIGN_BIT+EXPONENT_BIT+FRACTION_BIT);
            bitset<SIGN_BIT+EXPONENT_BIT+FRACTION_BIT> n3("1" + e2.to_string() + f2.to_string(), 0, SIGN_BIT+EXPONENT_BIT+FRACTION_BIT);
            inputfile << n1.to_string() << " " << n2.to_string() << endl;
            inputfile << n1.to_string() << " " << n3.to_string() << endl;

            count+=2;
        }
    }

    cout << "Total test cases generated: " << count << endl;
    inputfile.close();
}