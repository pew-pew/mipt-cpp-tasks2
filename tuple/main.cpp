using namespace std;
#include <iostream>
#include <fstream>
#include "tuple.h"


struct S {
    S() {
        cout << "S()" << endl;
    }
    
    S(const S&) {
        cout << "S(const S&)" << endl;
    }
    
    S(S&&) {
        cout << "S(S&&)" << endl;
    }

    S& operator=(const S&) {
        cout << "=(const S&)" << endl;
        return *this;
    }
    
    S& operator=(S&&) {
        cout << "=(S&&)" << endl;
        return *this;
    }
};

template class Tuple<int, float>;

int main() {
    typedef Tuple<int, float, S, S, string> Tu;
    S s;
    Tu tt(1, 5, std::move(s), S(), "12312awd3");
    cout << "---" << endl;
    Tu b = tt;
    (void)b;
    cout << "--" << endl;

    cout << gett<1>(b) << endl;

    cout << boolalpha << (makeTuple(1, 2) < makeTuple(1, 2)) << endl;
}
