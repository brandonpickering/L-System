#pragma once


#include <functional>
#include <map>
#include <string>
#include <vector>


namespace lsystem {


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

    void addRule(Rule rule);
    std::vector<ParseError> interpret(std::vector<std::string> lines);

    String seed(std::string name = "seed") const;
    String iterate(String str, unsigned int iterations=1) const;
};



struct PSymbol {
    std::string name;
    std::vector<double> args;
};

typedef std::vector<PSymbol> PString;


struct PForm {
    std::string name;
    std::vector<std::string> params;
};

typedef std::map<std::string, double> PEnv;

struct PEnvSymbol {
    std::string name;
    std::vector<std::function<double(PEnv)>> args;

    PSymbol evaluate(PEnv env) const;
};

typedef std::vector<PEnvSymbol> PEnvString;

struct PRule {
    PForm form;
    PEnvString product;

    PString evaluate(std::vector<double> args) const;
};


class PSystem {
public:
    std::map<std::string, PRule> rules;

    void addRule(PRule rule);
    std::vector<ParseError> interpret(std::vector<std::string> lines);

    PString seed(std::string name = "seed") const;
    PString iterate(PString str, unsigned int iterations=1) const;
};


}
