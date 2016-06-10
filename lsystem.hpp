#pragma once


#include <map>
#include <string>
#include <vector>


typedef std::string Symbol;
typedef std::vector<Symbol> String;


class System {
public:
    std::map<Symbol, String> rules;

    String iterate(String str, unsigned int iterations=1) const;
};
