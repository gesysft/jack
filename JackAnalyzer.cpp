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
    int c;
    int test = 0;

    while (--argc > 0 && (*++argv)[0] == '-') {
        while ((c = *++argv[0]) != '\0') {
            switch (c) {
            case 't':
                test = 1;
                break;
            default:
                break;
            }
        }
    }
    string ext = (test == 0) ? ".xml" : "T.xml";

    if (argc == 0) {
        CompilationEngine *eng = new CompilationEngine("", "", 1, 0);
        test == 1 ? eng->test() : eng->compileClass();
        delete eng;
        return 0;
    }
    for (; --argc >= 0; ++argv) {
        if (stat(*argv, &stbuf) == -1) {
            continue;
        }

        /* is not dir */
        if (!S_ISDIR(stbuf.st_mode)) {
            string ifname = *argv;
            if (ifname.find(".jack") == string::npos)
                continue;

            string ofname = ifname.substr(0, ifname.find_last_of(".")) + ext;
            CompilationEngine *eng = new CompilationEngine(ifname, ofname, 1, 0);
            printf("%s to %s\n", ifname.c_str(), ofname.c_str());
            test == 1 ? eng->test() : eng->compileClass();
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
            CompilationEngine *eng = new CompilationEngine(ifname, ofname, 1, 0);
            printf("%s to %s\n", ifname.c_str(), ofname.c_str());
            test == 1 ? eng->test() : eng->compileClass();
            delete eng;
        }
    }
    return 0;
}


