
#include "lsystem.hpp"


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
