
#include <L-System/l_system.hpp>

#include <cstdio>


using namespace std;
using namespace l_system;


static void print(PString str) {
    for (PSymbol sym : str) {
        printf("%s(", sym.name.c_str());
        for (unsigned int i = 0; i < sym.args.size(); i++) {
            printf("%.1f", sym.args[i]);
            if (i < sym.args.size() - 1) printf(", ");
        }
        printf(") ");
    }
    printf("\n");
}


int main() {
    PString str = { {"A", {3, 2}}, {"B", {4}} };
    print(str);

    PRule rule = {
        { "A", {"t", "s"} },
        {
            { "B", { [](PEnv env) { return env["t"] + env["s"]; } } },
            { "A", { [](PEnv env) { return env["t"] + env["s"]; }, [](PEnv env) { return env["s"]; } } }
        },
    };
    PSystem system;
    system.addRule(rule);
    print(system.iterate(str));

    return 0;
}
