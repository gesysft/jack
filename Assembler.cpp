/*
    chapter 4: Machine Language
    chapter 5: Computer Architecture
    chapter 6: Assembler  
*/

#include <stdio.h>
#include <algorithm>
#include <string>
#include <fstream>
#include <iostream>
#include <map>
#include <string.h>
#include <vector>
#include "ASMSymbolTable.h"
#include "ASMParser.h"
#include "Code.h"

using namespace std;

vector<string> load(string inname);
void assemble(string inname);

int main(int argc, char *argv[]) {
    if (argc == 1) {
        assemble("");
    } else {
        while (--argc > 0)
            assemble(*++argv);
    }
    return 0;
}

vector<string> load(string inname) {
    vector<string> rom; 
    static int start = 16;
    ASMSymbolTable *st = new ASMSymbolTable;
    ASMParser *ps = new ASMParser(inname);

    // replace Labels Symbol 
    for (size_t pc = 0; ps->advance() != -1; ) {
        if (ps->commandType() == L_COMMAND)
            st->addEntry(ps->symbol(), pc);
        else {
            rom.push_back(ps->command());
            pc++;
        }
    }
    
    // replace Variables Symbol
    for (size_t pc = 0; pc < rom.size(); ++pc) {
        ps->setCommand(rom[pc]);
        if (ps->commandType() == A_COMMAND && !isdigit(ps->symbol()[0])) {
            int addr;
            if (st->contains(ps->symbol())) 
                addr = st->getAddress(ps->symbol());
            else
                st->addEntry(ps->symbol(), addr = start++);
            rom[pc] = "@" + to_string(addr); 
        }
    }
    delete st;
    delete ps;
    return rom;
}

void assemble(string inname) {
    FILE *ofp;
    Code *cd = new Code;
    ASMParser *ps = new ASMParser;
    vector<string> rom = load(inname);
    string outname = inname.substr(0, inname.find_last_of('.')) + ".hack";

    if (inname == "")
        ofp = stdout;
    else if ((ofp = fopen(outname.c_str(), "w")) == NULL)
        fprintf(stderr, "open failed: %s\n", outname.c_str());

    for (auto &cmd : rom) {
        ps->setCommand(cmd);
        if (ps->commandType() == A_COMMAND)
            fprintf(ofp, "%s\n", bitset<16>(stoi(ps->symbol())).to_string().c_str());
        else if (ps->commandType() == C_COMMAND)
            fprintf(ofp, "111%s%s%s\n", cd->comp(ps->comp()).c_str(),
                cd->dest(ps->dest()).c_str(), cd->jump(ps->jump()).c_str());
    }
    if (ofp != stdout) {
        printf("%s to %s\n", inname.c_str(), outname.c_str());
        fclose(ofp);
    }
    delete ps;
    delete cd;
}

