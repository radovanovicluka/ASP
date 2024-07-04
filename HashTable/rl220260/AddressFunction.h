
#ifndef DZ3_ADDRESSFUNCTION_H
#define DZ3_ADDRESSFUNCTION_H

using Key = unsigned long;
using Address = int;
using Attempt = int;

using namespace std;

class AddressFunction {
public:
    virtual Address getAddress( Key k, Address a, Attempt i ) = 0;
};


class QuadraticHashing : public AddressFunction {
private:
    double c1 = 0, c2 = 0;
public:
    QuadraticHashing() {
        while ( c1 <= 0 || c2 <= 0 ) {
            cout << endl << "Unesite validne parametre adresne funkcije: " << endl;
            cin >> c1 >> c2;
        }
    };

    Address getAddress( Key k, Address a, Attempt i ) override {
        return a + i * c1 + i * i * c2;
    }
};

#endif //DZ3_ADDRESSFUNCTION_H
