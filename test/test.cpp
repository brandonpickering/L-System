
#include <L-System/lsystem.hpp>

#include <cstdio>


using namespace std;
using namespace lsystem;


static void print(PString str) {
    for (PSymbol sym : str) {
        printf("%s", sym.name.c_str());
        if (sym.args.size() > 0) {
            printf("(");
            for (unsigned int i = 0; i < sym.args.size(); i++) {
                printf("%f", sym.args[i]);
                if (i < sym.args.size() - 1)
                    printf(", ");
            }
            printf(")");
        }
        printf(" ");
    }
    printf("\n");
}


int main() {
    PSystem system;
    system.interpret({
        " #var k 1 ",
        " seed -> Fib(0, 1) ",
        "Fib(n, m) -> Fib(m, n m k * +)",
    });

    PString str = system.seed();
    print(str);
    for (int i = 0; i < 10; i++) {
        str = system.iterate(str);
        print(str);
    }

    return 0;
}
