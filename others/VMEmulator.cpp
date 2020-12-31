#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <stdio.h>
#include <unistd.h>
#include "../VMParser.h"

using namespace std;

#define debug(args...) fprintf(stderr, args)

struct command_t {
    int type;
    string cmd;
    string arg1;
    int arg2;
    string name; // xxx.vm to xxx
};

enum {SP = 0, LCL, ARG, THIS, THAT, TEMP, REGISTER=13, FRAME=13, RET=14, STATIC=16, STACK=256, HEAP=2048, IO=16384};
int RAM[32768];
int &sp = RAM[SP];
int &lcl = RAM[LCL];
int &arg = RAM[ARG];
int &thi = RAM[THIS];
int &tha = RAM[THAT];
int &frame = RAM[FRAME];
int &ret = RAM[RET];
int stack_begin = sp;
map<string, int> statictab;
int staticno = 0;

int pop() {
    if (sp > 0)
        return RAM[--sp];
    else {
        fprintf(stderr, "pop: empty\n");
        exit(1);
    }
}

void push(int x) {
    if (sp < 32768)
        RAM[sp++] = x;
    else {
        fprintf(stderr, "push: full\n");   
        exit(1);
    }
}

void set(int a, int x) {
    RAM[a] = x;
}

int get(int a) {
    return RAM[a];
}

vector<command_t> ROM;
int PC = 0;
unsigned long counter = 0;
map<string, int> ROMLabel;

vector<string> callStack;
vector<string> Stack;

void getprogram();
void show();

int main() {
    map<string, int> m{{"local", LCL}, {"argument", ARG}, {"this", THIS},
        {"that", THAT}, {"pointer", THIS}, {"temp", TEMP}, {"static", STATIC}};

    getprogram();

    // init
    set(0, 256);
    set(1, 300);
    set(2, 400);
    set(3, 3000);
    set(4, 3010);

    stack_begin = sp;

    // begin with Sys.init
    PC = 0;
    for (size_t i = 0; i < ROM.size(); ++i)
        if (ROM[i].arg1 == "Sys.init") {
            PC = i;
            break;
        }
    
    for (; PC < ROM.size(); ) {
        const int type = ROM[PC].type;
        const string cmd = ROM[PC].cmd;
        const string arg1 = ROM[PC].arg1;
        const int arg2 = ROM[PC].arg2;
        const string name = ROM[PC].name;

        show();
        counter++;
        if (type == C_PUSH) {
            if (arg1 == "local" || arg1 == "argument" || arg1 == "this" || arg1 == "that")
                push(get(get(m[arg1]) + arg2));
            else if (arg1 ==  "pointer" || arg1 == "temp")
                push(get(m[arg1] + arg2));
            else if (arg1 == "static") {
                string s = name + "." + to_string(arg2);
                if (statictab.find(s) == statictab.end()) {
                    statictab[s] = m[arg1] + staticno++;
                    cout << s << " " << (staticno - 1) << endl;
                }
                push(get(statictab[s]));
                
            } else if (arg1 == "constant")
                push(arg2);
            else
                debug("undefine\n");
            PC++;
        } else if (type == C_POP) {
            if (arg1 == "local" || arg1 == "argument" || arg1 == "this" || arg1 == "that")
                set(get(m[arg1]) + arg2, pop());
            else if (arg1 ==  "pointer" || arg1 == "temp")
                set(m[arg1] + arg2, pop());
            else if (arg1 == "static") {
                string s = name + "." + to_string(arg2);
                if (statictab.find(s) == statictab.end()) {
                    statictab[s] = m[arg1] + staticno++;
                    cout << s << " " << (staticno - 1) << endl;
                }
                set(statictab[s], pop());
            }
            else  
                debug("undefine\n");
            PC++;
        } else if (type == C_ARITHMETIC) {
            int op2;
            if (cmd == "add") {
                push(pop() + pop());
            } else if (cmd == "sub") {
                op2 = pop();
                push(pop() - op2);
            } else if (cmd == "and") {
                push(pop() & pop());
            } else if (cmd == "or") {
                push(pop() | pop());
            }  else if (cmd == "eq") {
                op2 = pop();
                push(pop() == op2 ? -1 : 0);
            }  else if (cmd == "lt") {
                op2 = pop();
                push(pop() < op2 ? -1 : 0);
            }  else if (cmd == "gt") {
                op2 = pop();
                push(pop() > op2 ? -1 : 0);
            } else if (cmd == "not") {
                push(~pop());
            } else if (cmd == "neg") {
                push(-pop());
            } else
                debug("undefine: %s\n", cmd.c_str());
            PC++;
        } else if (type == C_LABEL) {
            PC++;
        } else if (type == C_IF) {
            if (pop() != 0)
                PC = ROMLabel[arg1];
            else
                PC++;
        } else if (type == C_GOTO) {
            if (arg1.find("Sys.init") != string::npos)
                break;
            PC = ROMLabel[arg1];
        } else if (type == C_FUNCTION) {
            callStack.push_back(arg1);
            stack_begin = sp;
            for (int i = 0; i < arg2; ++i)
                push(0);
            PC++;
        } else if (type == C_RETURN) {
            callStack.pop_back();

            frame = lcl;                // FRAME = LCL
            ret = get(frame-5);         // RET = *(FRAME-5)
            RAM[arg] = pop();           // *ARG = pop()
            sp = arg + 1;               // SP = ARG + 1
            tha = get(frame-1);         // THAT = *(FRAME-1)
            thi = get(frame-2);         //THIS = *(FRAME-2)
            arg = get(frame-3);         // ARG = *(FRAME-3)
            lcl = get(frame-4);         // LCL = *(FRAME-4)
            PC = ret;                   // goto RET
            if (callStack.empty())
                break;
        } else if (type == C_CALL) {
            int addr;
            for (addr = PC + 1; addr < ROM.size(); ++addr)
                if (ROM[addr].type != C_LABEL)
                    break;
            push(addr);                 // push return-address
            push(lcl);                  // push LCL
            push(arg);                  // push ARG
            push(thi);                  // push THIS
            push(tha);                  // push THAT
            arg = sp - arg2 - 5;  // ARG = SP-n-5
            lcl = sp;                   // LCL = SP
            PC = ROMLabel[arg1];        // goto f
                                        // (return-address)
        } else {
            PC++;
            debug("unknown command : %d %s %s %d\n", type, cmd.c_str(), arg1.c_str(), arg2);
        }
    }
    show();
}

void show() {
    // clear
    //debug("\033[2J");
    debug("%-15s %lu\n", "Program", counter);
    for (int i = 0; i < ROM.size(); ++i) {
        //if (i < PC - 3 || i > PC + 3)
        //    continue;
            
        for (auto x : ROMLabel)
            if (i == x.second)
                debug("(%s)\n", x.first.c_str());
        debug("%7s %-7d %s %s %d\n", PC == i ? "*" : "", i, ROM[i].cmd.c_str(), ROM[i].arg1.c_str(), ROM[i].arg2);

        if (i == ROM.size() - 1) 
            for (auto x : ROMLabel)
                if (x.second >= ROM.size())
                    debug("(%s)\n", x.first.c_str());
    }
    debug("%-15s [%d %d)\n", "Stack", stack_begin, sp);
    for (int i = stack_begin; i < sp; ++i)
        debug("%-15s %d\n", "", get(i));

    debug("CallStack\n");
    for (auto x : callStack) debug("%-15s %s\n", "", x.c_str());

    debug("RAM\n");
    for (int i = 0; i < sizeof(RAM) / sizeof(RAM[0]); ++i) {
        if (i == SP)
            debug("%7s %-7d %d\n", "SP", i, get(i));
        else if (i == get(SP))
            debug("%7s %-7d %d\n", "sp", i, get(i));
        else if (i == ARG)
            debug("%7s %-7d %d\n", "ARG", i, get(i));
        else if (i == get(ARG))
            debug("%7s %-7d %d\n", "arg", i, get(i));
        else if (i == LCL)
            debug("%7s %-7d %d\n", "LCL", i, get(i));
        else if (i == get(LCL))
            debug("%7s %-7d %d\n", "lcl", i, get(i));
        else if (i == THIS)
            debug("%7s %-7d %d\n", "THIS", i, get(i));
        else if (i == get(THIS))
            debug("%7s %-7d %d\n", "this", i, get(i));
        else if (i == THAT)
            debug("%7s %-7d %d\n", "THAT", i, get(i));
        else if (i == get(THAT))
            debug("%7s %-7d %d\n", "that", i, get(i));
        else if (i >= TEMP && i < REGISTER && get(i))
            debug("%7s %-7d %d\n", string("temp" + to_string(i - TEMP)).c_str(), i, get(i));
        else if (i >= REGISTER && i < STATIC && get(i))
            debug("%7s %-7d %d\n", string("r" + to_string(i)).c_str(), i, get(i));
        else if (i >= STATIC && i < STACK && get(i))
            debug("%7s %-7d %d\n", string("s" + to_string(i - STATIC)).c_str(), i, get(i));
        //else if (i >= STACK && i < HEAP && get(i))
        //    debug("%7s %7d %d\n", string("sk" + to_string(i)).c_str(), i, get(i));
        else if (get(i) != 0)
            debug("%7s %-7d %d\n", "", i, get(i));
    }
    debug("\n");

    fflush(stdout);
    fflush(stderr);
    usleep(10000);
}

void getprogram() {
    string functionname;
    string name;
    VMParser *ps = new VMParser("");

    while (ps->advance() != EOF) {
        int type = ps->commandType();
        string cmd = ps->command();
        string arg1 = ps->arg1();
        int arg2 = ps->arg2();
        
    
        if (type == C_FUNCTION) {
            name = functionname = arg1;
            name = name.substr(0, name.find_last_of('.')); // name mean filename without ext
            ROMLabel[arg1] = ROM.size();
            ROM.push_back({type, cmd, arg1, arg2, name});
        }
        else if (type == C_LABEL) {
            string label = functionname + "$" + arg1;
            ROMLabel[label] = ROM.size();
            ROM.push_back({type, cmd, label, arg2, name});
        }
        else if (type == C_IF || type == C_GOTO) {
            string label = functionname + "$" + arg1;
            ROM.push_back({type, cmd, label, arg2, name});
        }
        else {
            ROM.push_back({type, cmd, arg1, arg2, name});
        }
    }
}

