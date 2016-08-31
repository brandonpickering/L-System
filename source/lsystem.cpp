
#include <L-System/lsystem.hpp>


using namespace std;


namespace lsystem {


Symbol EnvSymbol::evaluate(Env env) const {
    vector<double> rargs;
    for (auto arg : args)
        rargs.push_back(arg(env));
    return { name, rargs };
}

String Rule::evaluate(vector<double> args) const {
    Env env;
    for (unsigned int i = 0; i < form.params.size(); i++)
        env[form.params[i]] = i < args.size() ? args[i] : 0;

    String result;
    for (EnvSymbol sym : product)
        result.push_back(sym.evaluate(env));
    return result;
}



void System::addRule(Rule rule) {
    rules[rule.form.name] = rule;
}

String System::seed(std::string name) const {
    return iterate({{name, {}}}, 1);
}

String System::iterate(String str, unsigned int iterations) const {
    for (unsigned int i = 0; i < iterations; i++) {
        String result;

        for (Symbol symbol : str) {
            auto ruleItr = rules.find(symbol.name);
            if (ruleItr == rules.end()) {
                result.push_back(symbol);
            } else {
                String product = ruleItr->second.evaluate(symbol.args);
                result.insert(result.end(), product.begin(), product.end());
            }
        }

        str = result;
    }
    return str;
}


}
