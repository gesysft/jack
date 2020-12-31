#ifndef COMPILATIONENGINE
#define COMPILATIONENGINE
#include <stdio.h>
#include <string>
#include "JackTokenizer.h"
#include "VMWriter.h"
#include "SymbolTable.h"

class CompilationEngine {
public:
    CompilationEngine(std::string ifname = "", std::string ofname = "", int ifxml = 0, int ifvm = 1) {
        tk = new JackTokenizer(ifname);
        st = new SymbolTable;
        vm = new VMWriter(ofname, ifvm);
        indent = 0;
        expressionNum = 0;
        ifNum = 0;
        whileNum = 0;

        if (ifxml == 0)
            ofp = NULL;
        else if (ofname == "")
            ofp = stdout;
        else if ((ofp = fopen(ofname.c_str(), "w")) == NULL)
            fprintf(stderr, "error: open failed: %s\n", ofname.c_str());
    }

    void compileClass() {
        // 'class' className '{' classVarDec* subroutineDec* '}'
        tk->advance();
        outnone("class", 0);
        out("class");
        className = tk->token();
        out(tk->token());
        out("{");
        while (1) {
            if (tk->token() == "static" || tk->token() == "field") {
                compileClassVarDec();
            } else if (tk->token() == "constructor" || tk->token() == "function" || tk->token() ==  "method") {
                compileSubroutineDec();
            } else {
                break;
            }
        }
        out("}");
        outnone("class", 1);
    }

    void test() {
        outnone("tokens", 0, 0);
        tk->advance();
        while (tk->tokenType() != EOF) 
            out(tk->token());
        outnone("tokens", 1, 0);
    }
private:
    void compileClassVarDec() {
        // ('static' | 'field') type varName (',' varName)* ';'
        std::string name, type;
        int kind;

        outnone("classVarDec", 0);
        kind = (tk->token() == "static") ? KIND_STATIC : KIND_FIELD;
        out(tk->token());
        type = tk->token();
        out(tk->token());
        st->define(name = tk->token(), type, kind);
        out(tk->token()); 
        while (tk->token() == ",") {
            out(tk->token());
            st->define(name = tk->token(), type, kind);
            out(tk->token());
        }
        out(";");
        outnone("classVarDec", 1);
    }

    void compileSubroutineDec() {
        // ('constructor' | 'function' | 'method') ('void' | type) subroutineName '(' ParameterList ')'
        // subroutinebody
        outnone("subroutineDec", 0);
        ifNum = 0;
        whileNum = 0;


        st->startSubroutine();
        if ((subroutine = tk->token()) == "method")
            st->define("this", className, KIND_ARG);
        out(tk->token());
        subroutineType = tk->token();
        out(tk->token());
        subroutineName = tk->token();
        out(tk->token());
        out("("); 
        compileParameterList();
        out(")");
        compileSubroutineBody();
        outnone("subroutineDec", 1);
    }

    void compileParameterList() {
        // parameterList:   ((type varName)) (',' type varName)*)?
        outnone("parameterList", 0);
        std::string name, type;

        if (tk->token() != ")") {
            type = tk->token();
            out(tk->token());
            st->define(name = tk->token(), type, KIND_ARG);
            out(tk->token());
            while (tk->token() == ",") {
                out(",");
                type = tk->token();
                out(tk->token());
                st->define(name = tk->token(), type, KIND_ARG);
                out(tk->token()); 
            }
        }
        outnone("parameterList", 1);
    }

    void compileSubroutineBody() {
        // subroutineBody:   '{' varDec* statements '}'
        outnone("subroutineBody", 0);
        out("{");
        while (tk->token() == "var")
            compileVarDec();
        vm->writeFunction(className + "." + subroutineName, st->varCount(KIND_VAR));
        if (subroutine == "constructor") {
            vm->writePush(SEGMENT_CONST, st->varCount(KIND_FIELD));
            vm->writeCall("Memory.alloc", 1);
            vm->writePop(SEGMENT_POINTER, 0);
        } else if (subroutine == "method") {
            vm->writePush(SEGMENT_ARG, 0);
            vm->writePop(SEGMENT_POINTER, 0);
        } 
        compileStatements();
        out("}");
        outnone("subroutineBody", 1);
    }

    void compileSubroutineCall(std::string name) {
        // subroutineCall:   subroutineName '(' expressionList ')' |
        //                   (className | varName) '.' subroutineName '(' expressoinList ')'
        std::string subroutineName, objectName, varName;

        if (name == "") {
            subroutineName = objectName = varName = tk->token(); 
            out(tk->token()); 
        } else {
            subroutineName = objectName = varName = name; 
        }

        if (tk->token() == "(") { /* subroutineName '(' expression List ')' */
            out("(");
            vm->writePush(SEGMENT_POINTER, 0);
            compileExpressionList();
            vm->writeCall(className + "." + subroutineName, ++expressionNum);
            out(")");
        }
        else if (tk->token() == ".") { /* (className | varName) '.' subroutineName '(' expressoinList ')' */
            out("."); 
            subroutineName = tk->token();
            out(tk->token());
            out("(");

            if (st->kindOf(objectName) == KIND_NONE) { // /* className */
                compileExpressionList();
                vm->writeCall(objectName + "." + subroutineName, expressionNum);
            } else { /* varName */
                objectName = st->typeOf(varName);
                vm->writePush(segmentOf(varName), st->indexOf(varName));
                compileExpressionList();
                vm->writeCall(objectName + "." + subroutineName, expressionNum + 1);
            }
            out(")");
        } 
    }

    void compileVarDec() {
        // varDec:   'var' type varName (',' varName)* ';'
        outnone("varDec", 0);
        std::string name, type;
        out("var"); 
        type = tk->token();
        out(tk->token());
        st->define(name = tk->token(), type, KIND_VAR);
        out(tk->token());
        while (tk->token() == ",") {
            out(","); 
            st->define(name = tk->token(), type, KIND_VAR);
            out(tk->token()); 
        }
        out(";");
        outnone("varDec", 1);
    }

    void compileStatements() {
        outnone("statements", 0);
        while (1) {
            if (tk->token() == "let")
                compileLet();
            else if (tk->token() == "if")
                compileIf();
            else if (tk->token() == "while")
                compileWhile();
            else if (tk->token() == "do")
                compileDo();
            else if (tk->token() == "return")
                compileReturn();
            else
                break;
        }
        outnone("statements", 1);
    }

    void compileDo() {
        // doStatement:   'do' subroutineCall ';'
        outnone("doStatement", 0);
        out("do");
        compileSubroutineCall("");
        vm->writePop(SEGMENT_TEMP, 0);
        out(";");
        outnone("doStatement", 1);
    }

    void compileLet() {
        // letStatement:   'let' varName ('[' expression ']')? '=' expression ';'
        std::string name;

        outnone("letStatement", 0);
        out("let"); 
        name = tk->token();
        out(tk->token());
        if (tk->token() == "[") {
            out("["); 
            compileExpression();
            vm->writePush(segmentOf(name), st->indexOf(name));
            vm->writeArithmetic("add");
            out("]"); 
            out("="); 
            compileExpression();
            vm->writePop(SEGMENT_TEMP, 0);    /* temp 0 = expression */
            vm->writePop(SEGMENT_POINTER, 1); /* top statck = RAM address of array */
            vm->writePush(SEGMENT_TEMP, 0);
            vm->writePop(SEGMENT_THAT, 0);
            out(";");
        } else {
            out("="); 
            compileExpression();
            vm->writePop(segmentOf(name), st->indexOf(name));
            out(";");
        }
        outnone("letStatement", 1);
    }

    void compileWhile() {
        // whileStatement:   'while' '(' expression ')' '{' statements '}'
        int num = whileNum++;
        outnone("whileStatement", 0);
        out("while");
        out("(");
        vm->writeLabel("WHILE_EXP" + std::to_string(num));
        compileExpression();
        vm->writeArithmetic("not");
        vm->writeIf("WHILE_END" + std::to_string(num));
        out(")"); 
        out("{"); 
        compileStatements();
        vm->writeGoto("WHILE_EXP" + std::to_string(num));
        vm->writeLabel("WHILE_END" + std::to_string(num));
        out("}"); 
        outnone("whileStatement", 1);
    }

    void compileReturn() {
        // returnStatement:   'return' expression? ';'
        outnone("returnStatement", 0);
        out("return");
        if (tk->token() != ";") {
            compileExpression();
        } else {
            vm->writePush(SEGMENT_CONST, 0);   
        }
        vm->writeReturn();
        out(";");
        outnone("returnStatement", 1);
    }

    void compileIf() {
        // ifStatement:   'if' '(' expression ')' '{' statements '}' ('else' '{' statements '}')?
        int num = ifNum++;
        
        outnone("ifStatement", 0);
        out("if");
        out("(");
        compileExpression();
        out(")");
        out("{"); 
        vm->writeIf("IF_TRUE" + std::to_string(num));
        vm->writeGoto("IF_FALSE" + std::to_string(num));
        vm->writeLabel("IF_TRUE" + std::to_string(num));
        compileStatements();
        out("}");
        if (tk->token() == "else") {
            vm->writeGoto("IF_END" + std::to_string(num));
            out("else");
            out("{");
            vm->writeLabel("IF_FALSE" + std::to_string(num));
            compileStatements();
            out("}");
            vm->writeLabel("IF_END" + std::to_string(num));
        } else {
            vm->writeLabel("IF_FALSE" + std::to_string(num));
        }
        outnone("ifStatement", 1);
    }

    void compileExpression() {
        // expression:   term (op term)*
        // op:   '+' | '-' | '*' | '/' | '&' | '|' | '<' | '>' | '='
        static std::map<std::string, std::string> opstr{{"+", "add"}, {"-", "sub"}, {"&", "and"},
            {"|", "or"}, {"<", "lt"}, {">", "gt"}, {"=", "eq"}};
        std::string op;

        outnone("expression", 0);
        compileTerm();
        while (tk->token().find_first_of("+-*/&|<>=") != std::string::npos) {
            op = tk->token();
            out(tk->token());
            compileTerm();
            if (op == "*")
                vm->writeCall("Math.multiply", 2);
            else if (op == "/")
                vm->writeCall("Math.divide", 2);
            else
                vm->writeArithmetic(opstr[op]);
        }
        outnone("expression", 1);
    }
    void compileTerm() {
        std::string name;
        std::string unaryOp;
        outnone("term", 0);

        if (tk->tokenType() == INT_CONST) { /* interConstand */
            vm->writePush(SEGMENT_CONST, stoi(tk->token()));
            out(tk->token());
        } else if (tk->tokenType() == STRING_CONST) { /* std::stringConstant */
            vm->writePush(SEGMENT_CONST, tk->token().size());
            vm->writeCall("String.new", 1);
            for (int i = 0; i < tk->token().size(); ++i) {
                vm->writePush(SEGMENT_CONST, tk->token()[i]);
                vm->writeCall("String.appendChar", 2);
            }
            out(tk->token());
        } else if (tk->tokenType() == KEYWORD) { /* keywordConstant */
            if (tk->token() == "null" || tk->token() == "false")
                vm->writePush(SEGMENT_CONST, 0);
            else if (tk->token() == "true") {
                vm->writePush(SEGMENT_CONST, 0);
                vm->writeArithmetic("not");
            } else if (tk->token() == "this") {
                vm->writePush(SEGMENT_POINTER, 0);
            }
            out(tk->token());
        } else if (tk->token() == "(") { /* '(' expression ')' */
            out("("); 
            compileExpression();
            out(")");
        } else if (tk->token().find_first_of("-~") != std::string::npos) { /* unaryOp term */
            unaryOp = tk->token();
            out(tk->token());
            compileTerm();
            vm->writeArithmetic(unaryOp == "-" ? "neg" : "not");
        } else {
            name = tk->token();
            out(tk->token());
            if (tk->token() == "[") { /* varName '[' expression ']' */
                out("[");
                compileExpression();
                vm->writePush(segmentOf(name), st->indexOf(name)); /* array */
                vm->writeArithmetic("add");
                vm->writePop(SEGMENT_POINTER, 1);   
                vm->writePush(SEGMENT_THAT, 0);
                out("]");
            } else if (tk->token() == "(" || tk->token() == ".") { /* subroutineCall */
                compileSubroutineCall(name); 
            } else { /* varName */
                vm->writePush(segmentOf(name), st->indexOf(name));
            }
        }
        outnone("term", 1);
    }

    void compileExpressionList() {
        //expressionList:   (expression (',' expression)*)?
        outnone("expressionList", 0);
        expressionNum = 0;
        if (tk->token() != ")") {
            compileExpression();
            expressionNum++;
            while (tk->token() == ",") {
                out(",");
                compileExpression();
                expressionNum++;
            }
        }
        outnone("expressionList", 1);
    }

    void outnone(std::string s, int flag, int n = 2) {
        // output none terminals
        if (ofp == NULL)
            return;

        if (flag == 0) {
            fprintf(ofp, "%*.s<%s>\r\n", indent, " ", s.c_str());
            indent += n;
        }
        else {
            indent -= n;
            fprintf(ofp, "%*.s</%s>\r\n", indent, " ", s.c_str());
        }
    }

    void out(std::string s) {
        // output terminals
        std::string token = tk->token();
        static std::map<int, std::string> m {{KEYWORD, "keyword"}, {SYMBOL, "symbol"},
            {IDENTIFIER, "identifier"}, {INT_CONST, "integerConstant"}, {STRING_CONST, "stringConstant"}};
        static std::map<std::string, std::string> symbol{{"<", "&lt;"}, {">", "&gt;"}, {"&", "&amp;"}};

        if (tk->token() != s) {
            fprintf(stderr, "error: %s != %s\n", tk->token().c_str(), s.c_str());
            exit(1);
        }

        if (tk->tokenType() == SYMBOL && symbol.find(tk->token()) != symbol.end())
            token = symbol[tk->token()];
            
        if (ofp != NULL)
            fprintf(ofp, "%*.s<%s> %s </%s>\r\n", indent, " ",
                m[tk->tokenType()].c_str(), token.c_str(), m[tk->tokenType()].c_str());
        tk->advance();
    }

    /* segmentOf: return the segment of the named identifier */
    int segmentOf(std::string name) {
        std::map<int, int> m{{KIND_STATIC, SEGMENT_STATIC}, {KIND_FIELD, SEGMENT_THIS}, {KIND_ARG, SEGMENT_ARG}, 
            {KIND_VAR, SEGMENT_LOCAL}, {KIND_NONE, SEGMENT_NONE}};
        return m[st->kindOf(name)];
    }

    void close() {
        delete tk;
        delete st;
        delete vm;
        if (ofp != NULL && ofp != stdout)
            fclose(ofp);
    }
private:
    JackTokenizer *tk;
    SymbolTable *st;
    VMWriter *vm;
    FILE *ofp;

    std::string className;
    std::string subroutine;      /* constructor, method, function */
    std::string subroutineType;  /* int, void, ... */
    std::string subroutineName;  
    int expressionNum;
    int ifNum;
    int whileNum;
    int indent;
};

#endif
