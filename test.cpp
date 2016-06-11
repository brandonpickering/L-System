
#include "lsystem.hpp"

#include <cstdio>


using namespace std;


static void print(String str) {
    for (Symbol sym : str)
        printf("%s ", sym.c_str());
    printf("\n");
}


int main() {
    System system;
    vector<ParseError> errors = system.interpret({ "#seed O", "I -> I I", "O -> I[O]O" });

    if (errors.size() > 0) {
        for (ParseError error : errors)
            printf("%d:%d: %s\n", error.line, error.column, error.message.c_str());
    
    } else {
        String str = system.seed;
        print(str);
        for (int i = 0; i < 3; i++) {
            str = system.iterate(str);
            print(str);
        }
    }

    return 0;
}
