
#include <L-System/lsystem.hpp>

#include <cctype>
#include <cstdio>
#include <stack>


using namespace std;


namespace lsystem {


static void eatWhitespace(string input, unsigned int &index) {
    while (index < input.length() && isspace(input[index])) index++;
}


static void matchString(string pattern, string input, unsigned int &index, vector<ParseError> &errors) {
    string candidate = input.substr(index, pattern.size());
    if (candidate != pattern)
        errors.push_back({0, index, "expected '" + pattern + "'"});
    index += candidate.size();
}

static void pParseString(string pattern, string input, unsigned int &index, vector<ParseError> &errors) {
    matchString(pattern, input, index, errors);
    eatWhitespace(input, index);
}


static bool isVar(char c) {
    return isalpha(c) || c == '_';
}

// Assumes isVar passes
static string _pParseVar(string input, unsigned int &index) {
    string result;
    while (index < input.length() && (isalnum(input[index]) || input[index] == '_'))
        result.push_back(input[index++]);

    eatWhitespace(input, index);
    return result;
}

static string pParseVar(string input, unsigned int &index, vector<ParseError> &errors) {
    if (index >= input.length()) {
        errors.push_back({0, index, "expected variable, found end of line"});
        return "";
    }
    if (!isVar(input[index])) {
        errors.push_back({0, index, "expected variable, found '" + string(1, input[index]) + "'"});
        return "";
    }
    return _pParseVar(input, index);
}

static bool isNum(string input, unsigned int index) {
    if (index >= input.length()) return false;
    if (input[index] == '-') index++;
    if (index >= input.length()) return false;
    if (input[index] == '.') index++;
    if (index >= input.length()) return false;
    return isdigit(input[index]);
}

// Assumes isNum passes
static string _pParseNum(string input, unsigned int &index) {
    bool neg = false;
    if (input[index] == '-') { neg = true; index++; }

    string result;
    int numDec = 0;
    while (index < input.length() && (isdigit(input[index]) || (numDec == 0 && input[index] == '.'))) {
        if (input[index] == '.') numDec++;
        result.push_back(input[index++]);
    }

    eatWhitespace(input, index);
    return neg ? "-" + result : result;
}

static string pParseNum(string input, unsigned int &index, vector<ParseError> &errors) {
    if (index >= input.length()) {
        errors.push_back({0, index, "expected number, found end of line"});
        return "";
    }
    if (!isNum(input, index)) {
        errors.push_back({0, index, "expected number, found '" + string(1, input[index]) + "'"});
        return "";
    }
    return _pParseNum(input, index);
}

static bool isOpSym(char c) {
    return string("+-/*").find(c) != string::npos;
}

static vector<string> pParseArithExpr(string input, unsigned int &index, vector<ParseError> &errors) {
    if (index >= input.length()) {
        errors.push_back({0, index, "expected arithmetic expression, found end of line"});
        return {};
    }

    vector<string> result;

    while (index < input.length()) {
        if (isVar(input[index])) {
            result.push_back(_pParseVar(input, index));
        } else if (isNum(input, index)) {
            result.push_back(_pParseNum(input, index));
        } else if (isOpSym(input[index])) {
            result.push_back(string(1, input[index++])); eatWhitespace(input, index);
        } else {
            break;
        }
    }

    if (result.size() == 0)
        errors.push_back({0, index, "expected arithmetic expression, found '" + string(1, input[index]) + "'"});
    return result;
}

static double evaluate(vector<string> expr, Env globalEnv, Env localEnv) {
    stack<double> evalStack;

    auto pop = [&evalStack]() {
        if (evalStack.empty()) return 0.0;
        double val = evalStack.top();
        evalStack.pop();
        return val;
    };

    for (string term : expr) {
        if (term == "+") {
            double b = pop(), a = pop();
            evalStack.push(a + b);
        } else if (term == "-") {
            double b = pop(), a = pop();
            evalStack.push(a - b);
        } else if (term == "*") {
            double b = pop(), a = pop();
            evalStack.push(a * b);
        } else if (term == "/") {
            double b = pop(), a = pop();
            evalStack.push(a / b);
        } else if (term == "neg") {
            double a = pop();
            evalStack.push(-a);
        } else if (term == "inv") {
            double a = pop();
            evalStack.push(1/a);
        
        } else if (isVar(term[0])) {
            if (localEnv.find(term) != localEnv.end())
                evalStack.push(localEnv.at(term));
            else
                evalStack.push(globalEnv[term]);
        
        } else {
            evalStack.push(stod(term));
        }
    }

    return pop();
}


static bool isSymbolName(char c) {
    return !isspace(c) && !isdigit(c) && c != '#' && c != '(' && c != ')';
}

static string pParseSymbolName(string input, unsigned int &index, vector<ParseError> &errors) {
    if (index >= input.length()) {
        errors.push_back({0, index, "expected symbol, found end of line"});
        return "";
    }
    if (!isSymbolName(input[index])) {
        errors.push_back({0, index, "expected symbol, found '" + string(1, input[index]) + "'"});
        return "";
    }

    string result;

    if (index < input.length() && !(isalpha(input[index]) || input[index] == '_')) {
        result.push_back(input[index++]);
    } else {
        while (index < input.length() && (isalnum(input[index]) || input[index] == '_'))
            result.push_back(input[index++]);
    }

    eatWhitespace(input, index);
    return result;
}


static EnvSymbol pParseEnvSymbol(string input, unsigned int &index, vector<ParseError> &errors, Env env) {
    EnvSymbol result;
    result.name = pParseSymbolName(input, index, errors);
    
    vector<vector<string>> args;
    if (input[index] == '(') {
        pParseString("(", input, index, errors);

        while (input[index] != ')') {
            if (args.size() != 0) pParseString(",", input, index, errors);
            args.push_back(pParseArithExpr(input, index, errors));
            if (errors.size() > 0) break;
        }
        
        pParseString(")", input, index, errors);
    }

    for (vector<string> arg : args) {
        result.args.push_back([arg, env](Env localEnv) {
            return evaluate(arg, env, localEnv);
        });
    }

    return result;
}


static EnvString pParseEnvString(string input, unsigned int &index, vector<ParseError> &errors, Env env) {
    EnvString result;
    while (index < input.length() && isSymbolName(input[index])) {
        result.push_back(pParseEnvSymbol(input, index, errors, env));
    }
    return result;
}


static Form pParseForm(string input, unsigned int &index, vector<ParseError> &errors) {
    Form result;
    result.name = pParseSymbolName(input, index, errors);
    
    if (input[index] == '(') {
        pParseString("(", input, index, errors);

        while (input[index] != ')') {
            if (result.params.size() != 0) pParseString(",", input, index, errors);
            result.params.push_back(pParseVar(input, index, errors));
            if (errors.size() > 0) break;
        }
        
        pParseString(")", input, index, errors);
    }

    return result;
}


static Rule pParseRule(string input, unsigned int &index, vector<ParseError> &errors, Env env) {
    Form form = pParseForm(input, index, errors);
    pParseString("->", input, index, errors);
    EnvString product = pParseEnvString(input, index, errors, env);
    return { form, product };
}


static vector<ParseError> interpretLine(string input, System &system, Env &env) {
    vector<ParseError> errors;
    unsigned int index = 0;

    eatWhitespace(input, index);
    if (index >= input.length()) return errors;

    if (input[index] == '#') {
        index++;
        unsigned int index0 = index;

        unsigned int ws = index;
        while (ws < input.size() && !isspace(input[ws])) ws++;
        string command = input.substr(index, ws - index);
        index = ws; eatWhitespace(input, index);

        if (command == "var") {
            string var = pParseVar(input, index, errors);
            string num = pParseNum(input, index, errors);
            env[var] = errors.size() == 0 ? stod(num) : 0;
        
        } else {
            errors.push_back({0, index0, "unknown command '" + command + "'"});
        }

    } else {
        system.addRule(pParseRule(input, index, errors, env));
    }

    if (index < input.length())
        errors.push_back({0, index, "expected end of line, found '" + string(1, input[index]) + "'"});

    return errors;
}


static vector<ParseError> interpretLine(vector<string> linearts, System &system, Env &env) {
    string input;
    for (string &part : linearts) input += part;

    vector<ParseError> errors = interpretLine(input, system, env);
    for (ParseError &error : errors) {
        for (unsigned int i = 0; i < linearts.size() - 1; i++) {
            string part = linearts[i];
            if (error.column >= part.length()) {
                error.column -= part.length();
                error.line += 1;
            } else {
                break;
            }
        }
    }

    return errors;
}


static vector<ParseError> interpretLines(vector<string> lines, System &system, Env &env) {
    vector<ParseError> errors;
    lines.push_back("");

    unsigned int lineNum = 0;
    vector<string> linearts;

    for (string line : lines) {
        bool cont = false;
        if (line.length() != 0) {
            unsigned int i = line.length() - 1;
            while (i >= 1 && isspace(line[i])) i--;
            if (line[i] == '\\') {
                cont = true;
                line = line.substr(0, i);
            }
        }

        linearts.push_back(line + '\n');
        
        if (!cont) {
            vector<ParseError> lineErrors = interpretLine(linearts, system, env);
            linearts.clear();
            for (ParseError error : lineErrors) {
                error.line += lineNum;
                errors.push_back(error);
            }
        }
        
        lineNum++;
    }

    return errors;
}


vector<ParseError> System::interpret(vector<string> lines) {
    System system = *this;
    Env env;
    vector<ParseError> errors = interpretLines(lines, system, env);
    if (errors.size() == 0)
        *this = system;
    return errors;
}


}

