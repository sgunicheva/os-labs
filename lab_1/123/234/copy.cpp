#include <iostream>
#include <limits>
using namespace std;

double correct_n() {
    double vnum;
    while (!(cin >> vnum) || vnum <= 0 || cin.get() != '\n') {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Incorrect input!\nTry again\n";
    }
    return vnum;
}

bool isPowerOfTwo(double n, int &exp) {
    int p = 1;
    exp = 0;
    while (p < n) {
        p *= 2;
        exp++;
    }
    return p == n;
}

bool isPowerOfThree(double n, int &exp) {
    int p = 1;
    exp = 0;
    while (p < n) {
        p *= 3;
        exp++;
    }
    return p == n;
}

int main() {
    char input;
    do {
        int exponent;
        cout<<"Enter a number: ";
        double n = correct_n();


        cout<<"Выберете степень:";
        char choose;
        cin>>choose;
        if (choose=='2') {
            if (isPowerOfTwo(n, exponent)) {
                cout << "Very good!! That's two to the power of " << exponent << "!!" << endl;
            } else {
                cout << "Oh no!! It's not a power of two!!" << endl;
            }
        }
        if (choose=='3') {
            if (isPowerOfThree(n, exponent)) {
                cout << "Very good!! That's three to the power of " << exponent << "!!" << endl;
            } else {
                cout << "Oh no!! It's not a power of two!!" << endl;
            }
        }
        cout << "Would you like to try again? Enter y (english and small only): ";
        cin >> input;
    } while (input == 'y');

    return 0;
}
