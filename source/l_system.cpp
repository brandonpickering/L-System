
#include <L-System/l_system.hpp>


using namespace std;


namespace l_system {


void System::addRule(Rule rule) {
    rules[rule.form] = rule;
}

String System::iterate(String str, unsigned int iterations) const {
    for (unsigned int i = 0; i < iterations; i++) {
        String result;

        for (Symbol symbol : str) {
            auto ruleItr = rules.find(symbol);
            if (ruleItr == rules.end()) {
                result.push_back(symbol);
            } else {
                String product = ruleItr->second.product;
                result.insert(result.end(), product.begin(), product.end());
            }
        }

        str = result;
    }
    return str;
}



PSymbol PEnvSymbol::evaluate(PEnv env) const {
    vector<double> rargs;
    for (auto arg : args)
        rargs.push_back(arg(env));
    return { name, rargs };
}

PString PRule::evaluate(vector<double> args) const {
    PEnv env;
    for (unsigned int i = 0; i < form.params.size(); i++)
        env[form.params[i]] = i < args.size() ? args[i] : 0;

    PString result;
    for (PEnvSymbol sym : product)
        result.push_back(sym.evaluate(env));
    return result;
}



void PSystem::addRule(PRule rule) {
    rules[rule.form.name] = rule;
}

PString PSystem::iterate(PString str, unsigned int iterations) const {
    for (unsigned int i = 0; i < iterations; i++) {
        PString result;

        for (PSymbol symbol : str) {
            auto ruleItr = rules.find(symbol.name);
            if (ruleItr == rules.end()) {
                result.push_back(symbol);
            } else {
                PString product = ruleItr->second.evaluate(symbol.args);
                result.insert(result.end(), product.begin(), product.end());
            }
        }

        str = result;
    }
    return str;
}


}
