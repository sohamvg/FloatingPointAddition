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

#define TESTS 100
#define EXPONENT_TESTS 10

int main()
{
    vector<int> exponents = {0, 1, 9, 10, 99, 100, 101, 126, 127, 128, 254, 255};

    for (int i = 0; i < EXPONENT_TESTS; i++)
    {
        int num = rand() % (255 + 1);
        if (std::find(exponents.begin(), exponents.end(), num) == exponents.end()) {
            exponents.push_back(num); 
        }
    }
    
    ofstream inputfile, ansfile;
    inputfile.open("rand_inp.txt");
    int count = 0;
    
    for (int i = 0; i < TESTS; i++)
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

    cout << "Total test cases generated: " << count << endl;
    inputfile.close();
}