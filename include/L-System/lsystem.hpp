#pragma once


#include <functional>
#include <map>
#include <string>
#include <vector>


// TODO: Conditionals, probabilities, environment callbacks?


namespace lsystem {


struct ParseError {
    unsigned int line;
    unsigned int column;
    std::string message;
};



struct Symbol {
    std::string name;
    std::vector<double> args;
};

typedef std::vector<Symbol> String;


struct Form {
    std::string name;
    std::vector<std::string> params;
};

typedef std::map<std::string, double> Env;

struct EnvSymbol {
    std::string name;
    std::vector<std::function<double(Env)>> args;

    Symbol evaluate(Env env) const;
};

typedef std::vector<EnvSymbol> EnvString;

struct Rule {
    Form form;
    EnvString product;

    String evaluate(std::vector<double> args) const;
};


class System {
public:
    void addRule(Rule rule);
    std::vector<ParseError> interpret(std::vector<std::string> lines);

    String seed(std::string name = "seed") const;
    String iterate(String str, unsigned int iterations=1) const;

private:
    std::map<std::string, Rule> rules;
};


}
