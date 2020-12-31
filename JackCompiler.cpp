#include <string>
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include "CompilationEngine.h"

using namespace std;

int main(int argc, char *argv[]) {
    DIR *dirp;
    struct dirent *dp;
    struct stat stbuf;
    string ext = ".vm";

    if (argc == 1) {
        CompilationEngine *eng = new CompilationEngine;
        eng->compileClass();
        delete eng;
        return 0;
    }
    for (; --argc > 0;) {
        if (stat(*++argv, &stbuf) == -1) {
            continue;
        }

        /* is not dir */
        if (!S_ISDIR(stbuf.st_mode)) {
            string ifname = *argv;
            if (ifname.find(".jack") == string::npos)
                continue;

            string ofname = ifname.substr(0, ifname.find_last_of(".")) + ext;
            CompilationEngine *eng = new CompilationEngine(ifname, ofname);
            printf("%s to %s\n", ifname.c_str(), ofname.c_str());
            eng->compileClass();
            delete eng;
        } 

        // is dir  
        if ((dirp = opendir(*argv)) == NULL)
            continue;
        while ((dp = readdir(dirp))) {
            string ifname = string(*argv) + "/" + dp->d_name;
            if (ifname.find(".jack") == string::npos
                || stat(ifname.c_str(), &stbuf) == -1 || S_ISDIR(stbuf.st_mode))
                continue;

            string ofname = ifname.substr(0, ifname.find_last_of(".")) + ext;
            CompilationEngine *eng = new CompilationEngine(ifname, ofname);
            printf("%s to %s\n", ifname.c_str(), ofname.c_str());
            eng->compileClass();
            delete eng;
        }
    }
    return 0;
}


