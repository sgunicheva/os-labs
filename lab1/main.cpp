#include <iostream>
#include <limits>
using namespace std;

int correct_n() {
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

int main() {
    char input;
    do {
        int exponent;
        double n = correct_n();

        if (isPowerOfTwo(n, exponent)) {
            cout << "О!! Вери гуд!! Итс два в степени " << exponent << "!!" << endl;
        } else {
            cout << "Оу ноу!! Этс нот степень двойка!!" << endl;
        }

        cout << "Хотите ещё раз? Нажмите y (только английскую): ";
        cin >> input;
    } while (input == 'y');

    return 0;
}
