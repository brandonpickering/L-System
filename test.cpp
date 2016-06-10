
#include "lsystem.hpp"

#include <cstdio>


using namespace std;


void print(String str) {
    for (Symbol sym : str)
        printf("%s ", sym.c_str());
    printf("\n");
}


int main() {
    System system;
    system.rules["A"] = {"A", "B"};
    system.rules["B"] = {"C", "B"};
    system.rules["C"] = {};

    String start = {"A"};
    for (int i = 0; i < 10; i++) {
        print(start);
        start = system.iterate(start);
    }

    return 0;
}
