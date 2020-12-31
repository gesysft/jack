#ifndef VMPARSER_H
#define VMPARSER_H
#include <map>
#include <string>
#include <stdio.h>

enum {C_ARITHMETIC, C_PUSH, C_POP, C_LABEL, C_GOTO, C_IF, C_FUNCTION, C_RETURN, C_CALL};

class VMParser {
public:
    std::string infile;

    VMParser(std::string name = "") {
        if (name == "") {
            ifp = stdin;
            infile = "stdin";
            return;
        }
        infile = name;
        if ((ifp = fopen(infile.c_str(), "r")) == NULL) {
            fprintf(stderr, "error: can't open %s for read\n", infile.c_str());
        }
    }

    bool hasMoreCommands() {
        return !feof(ifp);
    }

    int advance() {
        static std::map<std::string, int> m {
            {"add", C_ARITHMETIC}, {"sub", C_ARITHMETIC}, {"neg", C_ARITHMETIC},
            {"eq",  C_ARITHMETIC}, {"gt",  C_ARITHMETIC}, {"lt",  C_ARITHMETIC},
            {"and", C_ARITHMETIC}, {"or",  C_ARITHMETIC}, {"not", C_ARITHMETIC},
            {"push", C_PUSH},      {"pop", C_POP},        {"label", C_LABEL},
            {"goto", C_GOTO},      {"if-goto", C_IF},     {"function", C_FUNCTION},
            {"return", C_RETURN},  {"call", C_CALL}};
        char line[1024], cmd[1024], argument1[1024], argument2[1024];
        char *p;

        while (fgets(line, sizeof(line), ifp) != NULL) {
            if ((p = strstr(line, "//")) != NULL) {
                *p++ = '\n';
                *p = '\0';
            }
            *cmd = *argument1 = *argument2 = '\0';
            if (sscanf(line, "%s %s %s", cmd, argument1, argument2) <= 0)
                continue;

            if (m.find(cmd) == m.end()) {
                fprintf(stderr, "unknown command: %s\n", line);
                continue;
            }
            this->cmd = cmd;
            if ((type = m[this->cmd]) == C_ARITHMETIC || type == C_RETURN)
                this->argument1 = this->cmd;
            else
                this->argument1 = argument1;
            this->argument2 = atoi(argument2);
            return type;
        }
        return EOF;
    }

    int commandType() {
        return type;
    }

    std::string command() {
        return cmd;
    }

    std::string arg1() {
        return argument1;
    }

    int arg2() {
        return argument2;
    }

    ~VMParser() {
        if (ifp != stdin)
            fclose(ifp);
    }

private:
    FILE *ifp;
    int type;
    std::string cmd;
    std::string argument1;
    int argument2;
};

#endif
