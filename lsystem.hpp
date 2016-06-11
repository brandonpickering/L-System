#pragma once


#include <map>
#include <string>
#include <vector>


struct ParseError {
    unsigned int line;
    unsigned int column;
    std::string message;
};


typedef std::string Symbol;
typedef std::vector<Symbol> String;

struct Rule {
    Symbol form;
    String product;
};

class System {
public:
    std::map<Symbol, Rule> rules;
    String seed;

    void addRule(Rule rule);
    std::vector<ParseError> interpret(std::vector<std::string> lines);

    String iterate(String str, unsigned int iterations=1) const;
};
