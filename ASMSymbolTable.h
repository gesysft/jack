#ifndef ASMSYMBOLTABLE_H
#define ASMSYMBOLTABLE_H
#include <string>
#include <map>

class ASMSymbolTable {
public:
    ASMSymbolTable() {
        m = {{"SP", 0}, {"LCL", 1}, {"ARG", 2}, {"THIS", 3}, {"THAT", 4}, {"SCREEN", 16384},
            {"KEY", 24576}, {"R0", 0}, {"R1", 1}, {"R2", 2}, {"R3", 3}, {"R4", 4}, {"R5", 5},
            {"R6", 6}, {"R7", 7}, {"R8", 8}, {"R9", 9}, {"R10", 10}, {"R11", 11}, {"R12", 12},
            {"R13", 13}, {"R14", 14}, {"R15", 15}};
    }

    void addEntry(std::string symbol, int address) {
        m[symbol] = address;
    }

    bool contains(std::string symbol) {
        return m.find(symbol) != m.end();
    }

    int getAddress(std::string symbol) {
        return m[symbol];
    }

private:
    std::map<std::string, int> m;
};

#endif
