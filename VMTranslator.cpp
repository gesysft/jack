/*
    chapter7: Virtual Machine 1: Stack Arithmetic
    chapter8: Virtual Machine 2: Program Control
*/

#include <iostream>
#include <map>
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include "VMParser.h"
#include "CodeWriter.h"

using namespace std;

void translate(VMParser *ps, CodeWriter *cw);
string getname(char *path);

int main(int argc, char *argv[]) {
    DIR *dirp;
    struct dirent *dp;
    struct stat stbuf;
    
    if (argc == 1) {
        translate(new VMParser, new CodeWriter);
        return 1;
    }
    while (--argc > 0) {
        if (stat(*++argv, &stbuf) == -1) {
            continue;
        }

        // is not dir
        if (!S_ISDIR(stbuf.st_mode)) {
            string ifname = *argv;
            if (ifname.find(".vm") == string::npos)
                continue;

            string ofname = ifname.substr(0, ifname.find_last_of(".")) + ".asm";
            VMParser *ps = new VMParser(ifname);
            CodeWriter *cw = new CodeWriter(ofname);
            cw->setFileName(ifname);
            translate(ps, cw);
            printf("%s to %s\n", ps->infile.c_str(), cw->outfile.c_str());
            delete cw;
            delete ps;
        } 

        // is dir  
        if ((dirp = opendir(*argv)) == NULL)
            continue;
        string ofname = string(*argv) + "/" + getname(*argv) + ".asm";
        CodeWriter *cw = new CodeWriter(ofname);
        cw->writeInit();
        while ((dp = readdir(dirp))) {
            if (strstr(dp->d_name, ".vm") == NULL)
                continue;

            string ifname = string(*argv) + "/" + dp->d_name;
            if (stat(ifname.c_str(), &stbuf) == -1 || S_ISDIR(stbuf.st_mode))
                continue;

            VMParser *ps = new VMParser(ifname);
            cw->setFileName(dp->d_name);
            translate(ps, cw);
            printf("%s to %s\n", ps->infile.c_str(), cw->outfile.c_str());
            delete ps;
        }
        delete cw;
    }
    return 0;
}

void translate(VMParser *ps, CodeWriter *cw) {
    static string name = "null";
    while (ps->advance() != EOF) {
        int type = ps->commandType();
        string arg1 = ps->arg1();
        int arg2 = ps->arg2();
        
        if (type == C_PUSH || type == C_POP) {
            cw->writePushPop(type, arg1, arg2);
        } else if (type == C_ARITHMETIC) {
            cw->writeArithmetic(arg1);
        } else if (type == C_LABEL) {
            cw->writeLabel(name+ "$" + arg1);
        } else if (type == C_IF) {
            cw->writeIf(name + "$" + arg1);
        } else if (type == C_GOTO) {
            cw->writeGoto(name + "$" + arg1);
        } else if (type == C_FUNCTION) {
            name = arg1;
            cw->writeFunction(arg1, arg2);
        } else if (type == C_RETURN) {
            cw->writeReturn();
        } else if (type == C_CALL) {
            cw->writeCall(arg1, arg2);
        }
    }
}

string getname(char *p) {
    // get name form path/name/
    char s[1024];
    int i;
    for (i = 0; *p; p++) {
        if (*p == '/') {
            s[i] = '\0';
            i = 0;
        }
        else
            s[i++] = *p;
    }
    if (i != 0)
        s[i] = '\0';
    return s;
}

