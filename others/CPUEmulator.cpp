#include <stdio.h>
#include <algorithm>
#include <string>
#include <fstream>
#include <iostream>
#include <map>
#include <string.h>
#include <vector>
#include "../ASMParser.h"
#include "../ASMSymbolTable.h"
#include "../Code.h"

using namespace std;


ASMSymbolTable *st;
ASMParser *ps;
Code *cd;

vector<string> ROM;
unsigned long PC;
unsigned long PC_END = 0;
unsigned long counter = 0;

int RAM[32768];
int A;
int D;

struct {
    int d;
    int ma;
    string comp;
    int output;
} ALU;


void cpuemulate(string);
void show(int state = 0);
int alucomp(string comp);

int main(int argc, char *argv[]) {
    if (argc == 1) {
        cpuemulate("");
    } else {
        while (--argc > 0)
            cpuemulate(*++argv);
    }
    return 0;
}

void cpuemulate(string inname) {
    static int addr = 16;

    ps = new ASMParser(inname);
    st = new ASMSymbolTable;
    cd = new Code;

    // L_COMMAND, set address of label, fill rom
    for (PC = 0; ps->advance() != -1; ) {
        if (ps->commandType() == L_COMMAND) {
            st->addEntry(ps->symbol(), PC);
            if (ps->symbol().find("Sys.init") != string::npos)
                PC_END = PC;
        }
        else {
            ROM.push_back(ps->command());
            PC++;
        }
    }

    for (PC = 0; PC < ROM.size(); ) {
        show();
        counter++;
        ps->setCommand(ROM[PC]);
        
        // A_COMMAND 
        if (ps->commandType() == A_COMMAND) {
            if (isdigit(ps->symbol()[0]))
                A = stoi(ps->symbol());
            else {
                if (st->contains(ps->symbol()))
                    A = st->getAddress(ps->symbol());
                else
                    st->addEntry(ps->symbol(), A = addr++);
            }
            PC++;
            continue;
        }

        // C_COMMAND
        string comp = ps->comp();
        string dest = ps->dest();
        string jump = ps->jump();

        ALU.comp = comp;
        ALU.d = D;
        ALU.ma = ALU.comp.find('m') != string::npos ? RAM[A] : A;
        ALU.output = alucomp(ALU.comp);

        if (dest != "") {
            if (dest.find('D') != string::npos)
                D = ALU.output;
            if (dest.find('M') != string::npos) 
                RAM[A] = ALU.output;
            if (dest.find('A') != string::npos)
                A = ALU.output;
            PC++;
            continue;
        } 

        if ((jump == "JGT" && ALU.output > 0) || (jump == "JEQ" && ALU.output == 0)
            || (jump == "JGE" && ALU.output >= 0) || (jump == "JLT" && ALU.output < 0)
            || (jump == "JNE" && ALU.output != 0) || (jump == "JLE" && ALU.output <= 0)
            || (jump == "JMP"))
            PC = A;
        else
            PC++;

        if (PC == PC_END)
            break;
    }
    show(1);
}

int alucomp(string comp) {
    int n = 0;
    if (comp == "0")        n = 0;
    else if (comp == "1")   n = 1;
    else if (comp == "-1")  n = -1;
    else if (comp == "D")   n = D;
    else if (comp == "A")   n = A;
    else if (comp == "!D")  n = ~D;
    else if (comp == "!A")  n = ~A;
    else if (comp == "-D")  n = -D;
    else if (comp == "-A")  n = -A;
    else if (comp == "D+1") n = D+1;
    else if (comp == "A+1") n = A+1;
    else if (comp == "D-1") n = D-1;
    else if (comp == "A-1") n = A-1;
    else if (comp == "D+A") n = D+A;
    else if (comp == "A+D") n = A+D;
    else if (comp == "D-A") n = D-A;
    else if (comp == "A-D") n = A-D;
    else if (comp == "D&A") n = D&A;
    else if (comp == "A&D") n = A&D;
    else if (comp == "D|A") n = D|A;
    else if (comp == "A|D") n = A|D;
    else if (comp == "!M")  n = ~RAM[A];
    else if (comp == "M")   n = RAM[A];
    else if (comp == "-M")  n = -RAM[A];
    else if (comp == "M+1") n = RAM[A]+1;
    else if (comp == "M-1") n = RAM[A]-1;
    else if (comp == "D+M") n = D+RAM[A];
    else if (comp == "M+D") n = RAM[A]+D;
    else if (comp == "D-M") n = D-RAM[A];
    else if (comp == "M-D") n = RAM[A]-D;
    else if (comp == "D&M") n = D&RAM[A];
    else if (comp == "M&D") n = RAM[A]&D;
    else if (comp == "D|M") n = D|RAM[A];
    else if (comp == "M|D") n = RAM[A]|D;
    return n;
}

void show(int state) {
    if (counter == 0) {
        printf("%-7s %-7s %-7s %-7s %-7s %-7s %-7s %-7s %s\n",
            "COUNTER", "PC", "A", "D", "ALU.d", "ALU.ma", "COMP", "OUTPUT", "ASM");    
    }

    printf("%-7lu %-7lu %-7d ", counter, PC, A);
    if (ps->commandType() == A_COMMAND)
        printf("%32s", "");
    else
        printf("%-7d %-7d %-7s %-7d ", ALU.d, ALU.ma, ALU.comp.c_str(), ALU.output);
    

    printf("%s\n", PC < ROM.size() ? ROM[PC].c_str() : "");

    if (state == 1) {
        printf("RAM\n");
        for (size_t i = 0; i < sizeof(RAM) / sizeof(RAM[0]); ++i)
            if (RAM[i] != 0)
                printf("%7lu %d\n", i, RAM[i]);
    }
}
