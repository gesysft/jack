#ifndef VMWRITER_H
#define VMWRITER_H
#include <string>
#include <map>

enum { SEGMENT_CONST, SEGMENT_ARG, SEGMENT_LOCAL, SEGMENT_STATIC, SEGMENT_THIS,
        SEGMENT_THAT, SEGMENT_POINTER, SEGMENT_TEMP, SEGMENT_NONE };

static std::map<int, std::string> segmentStr{{SEGMENT_CONST, "constant"}, {SEGMENT_ARG, "argument"},
        {SEGMENT_LOCAL, "local"}, {SEGMENT_STATIC, "static"}, {SEGMENT_THIS, "this"}, {SEGMENT_THAT, "that"},
        {SEGMENT_POINTER, "pointer"}, {SEGMENT_TEMP, "temp"}, {SEGMENT_NONE, "none"}};

class VMWriter {
public:
    VMWriter(std::string name = "", int ifwrite = 1) {
        if (ifwrite == 0)
            ofp = NULL;
        else if (name == "") 
            ofp = stdout;
        else if ((ofp = fopen(name.c_str(), "w")) == NULL)
            fprintf(stderr, "error: can't open %s for write\n", name.c_str());
    }
    
    void writeCode(std::string s) {
        if (ofp != NULL) {
            fprintf(ofp, "%s\n", s.c_str());
        }
    }

    void writePush(int segment, int index) {
        writeCode("push " + segmentStr[segment] + " " + std::to_string(index));
    }

    void writePop(int segment, int index) {
        writeCode("pop " + segmentStr[segment] + " " + std::to_string(index));
    }

    void writeArithmetic(std::string command) {
        writeCode(command);
    }

    void writeLabel(std::string label) {
        writeCode("label " + label);
    }

    void writeGoto(std::string label) {
        writeCode("goto " + label);
    }

    void writeIf(std::string label) {
        writeCode("if-goto " + label);
    }

    void writeCall(std::string name, int nArgs) {
        writeCode("call " + name + " " + std::to_string(nArgs));
    }

    void writeFunction(std::string name, int nLocals) {
        writeCode("function " + name + " " + std::to_string(nLocals));
    }

    void writeReturn() {
        writeCode("return");
    }
    ~VMWriter() {
        if (ofp != NULL && ofp != stdout)
            fclose(ofp);
    }

private:
    FILE *ofp;
};
#endif
