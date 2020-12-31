#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H
#include <stdio.h>
#include <string>
#include <map>

enum { KIND_STATIC = 0, KIND_FIELD, KIND_ARG, KIND_VAR, KIND_NONE };

class SymbolTable {
public:
    /* startSubroutine: starts a new subroutine scope (i.e., resets the subroutine's symbole table) */
    void startSubroutine() {
        subroutineScope.clear();
    }

    /* define: Defines a new identifier of a give name, type, and kind and assigns it a runing index.
       STATIC and FIELD identifiers hava a class scope, while ARG and VAR identifiers have a subroutine scope. */
    void define(std::string name, std::string type, int kind) {
        if (kind == KIND_STATIC || kind == KIND_FIELD)
            classScope[name] = symbol{name, type, kind, varCount(kind)};
        else if (kind == KIND_ARG || kind == KIND_VAR)
            subroutineScope[name] = symbol{name, type, kind, varCount(kind)};
        else 
            fprintf(stderr, "define: unknown kind: %d\n", kind);
    }

    /* varCound: returns the number of variables of the given kind already defined in the current scope */
    int varCount(int kind) {
        int count = 0;

        for (auto x : subroutineScope)
            if (x.second.kind == kind)
                ++count;
        for (auto x : classScope)
            if (x.second.kind == kind)
                ++count;
        return count;
    }

    /* kindOf: returns the kind of the named identifier in the current scope. If the identifier is unknown
       in the current scope, returns NONE. */
    int kindOf(std::string name) {
        if (subroutineScope.find(name) != subroutineScope.end())
            return subroutineScope[name].kind;
        else if (classScope.find(name) != classScope.end())
            return classScope[name].kind;
        return KIND_NONE;
    }

    /* typeOf: returns the type of the named identifier in the current scope. */
    std::string typeOf(std::string name) {
        if (subroutineScope.find(name) != subroutineScope.end())
            return subroutineScope[name].type;
        else if (classScope.find(name) != classScope.end())
            return classScope[name].type;
        else
            fprintf(stderr, "typeOf: can't find the identifier: %s\n", name.c_str());
        return "none";
    }

    /* indexOf; returns the index of the named indentifier */
    int indexOf(std::string name) {
        if (subroutineScope.find(name) != subroutineScope.end())
            return subroutineScope[name].index;
        else if (classScope.find(name) != classScope.end())
            return classScope[name].index;
        else
            fprintf(stderr, "indexOf: can't find the identifier: %s\n", name.c_str());
        return -1; }

private:
    struct symbol {
        std::string  name;
        std::string type;
        int kind;
        int index;
    };
    std::map<std::string, symbol> classScope;
    std::map<std::string, symbol> subroutineScope;
};

#endif
