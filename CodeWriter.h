#ifndef CODEWRITE_H
#define CODEWRITE_H
#include <map>
#include <string>
#include <stdio.h>
#include "VMParser.h"

class CodeWriter {
public:
    std::string outfile;

public:
    CodeWriter(std::string name = "") {
        if (name == "") {
            outfile = "stdout";
            ofp = stdout;
            this->name = "stdin";
            return;
        }
        outfile = name;
        if ((ofp = fopen(outfile.c_str(), "w")) == NULL)
            fprintf(stderr, "error: can't open %s for write\n", outfile.c_str());
    }

    void setFileName(std::string fileName) {
        int i = fileName.find_last_of('/');
        int j = fileName.find_last_of('.');
        name = fileName.substr(i + 1, j - i - 1);
    }

    void writePushPop(int command, std::string segment, int index) {
        static std::map<std::string, std::string> m{
            {"local", "LCL"}, {"argument", "ARG"}, {"this", "THIS"},
            {"that", "THAT"}, {"pointer", "THIS"}, {"temp", "5"}};

        if (command == C_PUSH) {
            if (segment == "local" || segment == "argument" || segment == "this" || segment == "that")
                fprintf(ofp, "@%s\nD=M\n@%d\nA=D+A\nD=M\n", m[segment].c_str(), index);
            else if (segment == "pointer" || segment == "temp")
                fprintf(ofp, "@%s\nD=A\n@%d\nA=D+A\nD=M\n", m[segment].c_str(), index);
            else if (segment == "static") 
                fprintf(ofp, "@%s.%d\nD=M\n", name.c_str(), index); 
            else if (segment == "constant")
                fprintf(ofp, "@%d\nD=A\n", index);
            else {
                fprintf(stderr, "push: invalid segment: %s\n", segment.c_str());
                return;
            } 
            fprintf(ofp, "@SP\nA=M\nM=D\n@SP\nM=M+1\n"); // M[sp]=D, sp=sp+1
        } else if (command == C_POP) {
            if (segment == "local" || segment == "argument" || segment == "this" || segment == "that") {
                fprintf(ofp, "@%s\nD=M\n@%d\nD=D+A\n@R13\nM=D\n", m[segment].c_str(), index);
            } else if (segment == "pointer" || segment == "temp") {
                fprintf(ofp, "@%s\nD=A\n@%d\nD=D+A\n@R13\nM=D\n", m[segment].c_str(), index);
            } else if (segment == "static") 
                fprintf(ofp, "@%s.%d\nD=A\n@R13\nM=D\n", name.c_str(), index);
            else {
                fprintf(stderr, "pop: invalid segment: %s\n", segment.c_str());
                return;
            } 
            fprintf(ofp, "@SP\nM=M-1\nA=M\nD=M\n@R13\nA=M\nM=D\n"); // sp=sp-1, D=M[sp], M[M[R13]]=D
        }
    }

    void writeArithmetic(std::string command) {
        // add sub eq gt lt and or neg not
        static std::map<std::string, std::string> m{
            {"add", "M+D"}, {"sub", "M-D"}, {"and", "D&M"}, {"or", "D|M"},
            {"neg", "-M"},  {"not", "!M"},  {"eq", "JEQ"},  {"lt", "JLT"}, {"gt", "JGT"}};
        static int i = 0;

        if (command == "sub" || command == "add" || command == "or" || command == "and")
            fprintf(ofp, "@SP\nM=M-1\nA=M\nD=M\n@SP\nA=M-1\nM=%s\n", m[command].c_str());
        else if (command == "not" || command == "neg")
            fprintf(ofp, "@SP\nA=M-1\nM=%s\n", m[command].c_str());
        else if (command == "eq" || command == "lt" || command == "gt") {
            fprintf(ofp, "@SP\nM=M-1\nA=M\nD=M\n@SP\nA=M-1\nD=M-D\n");  // D = M[sp-1] - M[sp]
            fprintf(ofp, "@TRUE%d\nD;%s\nD=0\n@END%d\n0;JMP\n(TRUE%d)\nD=-1\n(END%d)\n",
                i, m[command].c_str(), i, i, i);                        // D = true ? -1 : 0;
            fprintf(ofp, "@SP\nA=M-1\nM=D\n");                          // M[sp-1] = D
            ++i;
        } else {
            fprintf(stderr, "arithmetic: invalid command: %s\n", command.c_str());
        }
    }

    void writeInit() {
        fprintf(ofp, "@256\nD=A\n@SP\nM=D\n");
        writeCall("Sys.init", 0);
    }

    void writeLabel(std::string label) {
        fprintf(ofp, "(%s)\n", label.c_str());
    }

    void writeGoto(std::string label) {
        fprintf(ofp, "@%s\n0,JMP\n", label.c_str());
    }

    void writeIf(std::string label) {
        fprintf(ofp, "@SP\nM=M-1\nA=M\nD=M\n@%s\nD;JNE\n", label.c_str());
    }

    void writeCall(std::string functionName, int numArgs) {
        static int i = 0;
        std::string addr = functionName + ".return." + std::to_string(i++);
        fprintf(ofp, "@%s\nD=A\n@SP\nA=M\nM=D\n@SP\nM=M+1\n", addr.c_str());    // push return-address
        fprintf(ofp, "@LCL\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n");                 // push LCL
        fprintf(ofp, "@ARG\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n");                 // push ARG
        fprintf(ofp, "@THIS\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n");                // push THIS
        fprintf(ofp, "@THAT\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n");                // push THAT
        fprintf(ofp, "@%d\nD=A\n@SP\nD=M-D\n@5\nD=D-A\n@ARG\nM=D\n", numArgs);  // ARG = SP - n - 5
        fprintf(ofp, "@SP\nD=M\n@LCL\nM=D\n");                                  // LCL = SP
        fprintf(ofp, "@%s\n0,JMP\n", functionName.c_str());                     // goto f
        fprintf(ofp, "(%s)\n", addr.c_str());                                   // (return-address)
    }

    void writeReturn() {
        fprintf(ofp, "@LCL\nD=M\n@R13\nM=D\n");                  // FRAME = LCL
        fprintf(ofp, "@5\nD=A\n@R13\nA=M-D\nD=M\n@R14\nM=D\n");  // RET = *(FRAME-5)
        fprintf(ofp, "@SP\nM=M-1\nA=M\nD=M\n@ARG\nA=M\nM=D\n");  // *ARG = pop()
        fprintf(ofp, "@ARG\nD=M+1\n@SP\nM=D\n");                 // SP = ARG + 1
        fprintf(ofp, "@1\nD=A\n@R13\nA=M-D\nD=M\n@THAT\nM=D\n"); // THAT = *(FRAME-1)
        fprintf(ofp, "@2\nD=A\n@R13\nA=M-D\nD=M\n@THIS\nM=D\n"); // THIS = *(FRAME-2)
        fprintf(ofp, "@3\nD=A\n@R13\nA=M-D\nD=M\n@ARG\nM=D\n");  // ARG = *(FRAME-3)
        fprintf(ofp, "@4\nD=A\n@R13\nA=M-D\nD=M\n@LCL\nM=D\n");  // LCL = *(FRAME-4)
        fprintf(ofp, "@R14\nA=M\n0,JMP\n");                      // goto RET
    }

    void writeFunction(std::string functionName, int numLocals) {
        fprintf(ofp, "(%s)\n", functionName.c_str());       // (f)
        while (numLocals-- > 0) 
            fprintf(ofp, "@SP\nA=M\nM=0\n@SP\nM=M+1\n");    // M[sp++] = 0;
    }

    void Close() {
        if (ofp != stdout)
            fclose(ofp);
    }
private:
    std::string name;
    FILE *ofp;
};

#endif
