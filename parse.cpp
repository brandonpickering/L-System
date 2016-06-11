
#include "lsystem.hpp"

#include <cctype>


using namespace std;


static void eatWhitespace(string input, unsigned int &index) {
    while (index < input.length() && isspace(input[index])) index++;
}


static void matchString(string pattern, string input, unsigned int &index, vector<ParseError> &errors) {
    string candidate = input.substr(index, pattern.size());
    if (candidate != pattern)
        errors.push_back({0, index, "expected '" + pattern + "'"});
    index += candidate.size();
}


static bool isSymbol(char c) {
    return !isspace(c) && !isdigit(c);
}

static Symbol parseSymbol(string input, unsigned int &index, vector<ParseError> &errors) {
    if (index >= input.length()) {
        errors.push_back({0, index, "expected symbol, found end of line"});
        return Symbol();
    }
    if (!isSymbol(input[index])) {
        errors.push_back({0, index, "expected symbol, found '" + string(1, input[index]) + "'"});
        return Symbol();
    }

    Symbol result;

    if (index < input.length() && !(isalpha(input[index]) || input[index] == '_')) {
        result.push_back(input[index++]);
    } else {
        while (index < input.length() && (isalnum(input[index]) || input[index] == '_'))
            result.push_back(input[index++]);
    }

    eatWhitespace(input, index);
    return result;
}


static String parseString(string input, unsigned int &index, vector<ParseError> &errors) {
    String result;
    while (index < input.length() && isSymbol(input[index])) {
        result.push_back(parseSymbol(input, index, errors));
    }
    return result;
}


static Rule parseRule(string input, unsigned int &index, vector<ParseError> &errors) {
    Symbol form = parseSymbol(input, index, errors);
    matchString("->", input, index, errors); eatWhitespace(input, index);
    String product = parseString(input, index, errors);
    return { form, product };
}


static vector<ParseError> interpretLine(string input, System &system) {
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

        if (command == "seed") {
            system.seed = parseString(input, index, errors);
        
        } else {
            errors.push_back({0, index0, "unknown command '" + command + "'"});
        }

    } else {
        system.addRule(parseRule(input, index, errors));
    }

    if (index < input.length())
        errors.push_back({0, index, "expected end of line, found '" + string(1, input[index]) + "'"});

    return errors;
}

static vector<ParseError> interpretLine(vector<string> lineParts, System &system) {
    string input;
    for (string &part : lineParts) input += part;

    vector<ParseError> errors = interpretLine(input, system);
    for (ParseError &error : errors) {
        for (unsigned int i = 0; i < lineParts.size() - 1; i++) {
            string part = lineParts[i];
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

static vector<ParseError> interpretLines(vector<string> lines, System &system) {
    vector<ParseError> errors;
    lines.push_back("");

    unsigned int lineNum = 0;
    vector<string> lineParts;

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

        lineParts.push_back(line + '\n');
        
        if (!cont) {
            vector<ParseError> lineErrors = interpretLine(lineParts, system);
            lineParts.clear();
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
    vector<ParseError> errors = interpretLines(lines, system);
    if (errors.size() == 0)
        *this = system;
    return errors;
}
