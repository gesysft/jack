#ifndef ASMPARSER_H
#define ASMPARSER_H
#include <string>
#include <stdio.h>

enum {A_COMMAND, C_COMMAND, L_COMMAND};

class ASMParser {
public:
    ASMParser(std::string name = "") {
        if (name == "")
            ifp = stdin;
        else if ((ifp = fopen(name.c_str(), "r")) == NULL)
            fprintf(stderr, "open failed: %s\n", name.c_str());
    }
    
    bool hasMoreCommands() {
        return !feof(ifp);
    }
    
    int advance() {
        char line[1024];
        cmd = "";
        while (fgets(line, sizeof(line), ifp) != NULL) {
            for (int i = 0; line[i] != '\0' && line[i] != '/'; ++i)
                if (!isspace(line[i]))
                    cmd.push_back(line[i]);
            if (cmd.empty())
                continue;
            return commandType();
        }
        return -1;
    }
    
    int commandType() {
        if (cmd[0] == '@')
            return A_COMMAND;
        else if (cmd[0] == '(')
            return L_COMMAND;
        else
            return C_COMMAND;
    }
    
    std::string symbol() {
        return commandType() == A_COMMAND ? 
            cmd.substr(1) : cmd.substr(1, cmd.find_last_of(')') - 1);
    }
    
    std::string dest() {
        size_t pos = cmd.find('=');
        return pos != std::string::npos ? cmd.substr(0, pos) : "";
    }
    
    std::string comp() {
        size_t pos;
        if ((pos = cmd.find('=')) != std::string::npos)
            return cmd.substr(pos + 1);
        else if ((pos = cmd.find(';')) != std::string::npos)
            return cmd.substr(0, pos);
        else
            return "";
    }
    
    std::string jump() {
        size_t pos = cmd.find(';');
        return pos != std::string::npos ? cmd.substr(pos + 1) : "";
    }
    
    std::string command() {
        return cmd;       
    }
    
    void setCommand(std::string command) {
        cmd = command;
    }
    
    ~ASMParser() {
        if (ifp != stdin)
            fclose(ifp);
    }
    
private:
    std::string cmd;
    FILE *ifp;
};

#endif
