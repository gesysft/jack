#ifndef JACKTOKENIZER_H
#define JACKTOKENIZER_H
#include <string>
#include <set>
#include <stdio.h>

enum Token {KEYWORD = 1, SYMBOL, IDENTIFIER, INT_CONST, STRING_CONST};
enum Keyword {CLASS, METHOD, FUNCTION, CONSTRUCTOR, INT, BOOLEAN, CHAR, VOID, VAR, STATIC, FIELD, LET, DO, IF, ELSE,
        WHILE, RETURN, TURE, FALSE, NULLNONE, THIS};

class JackTokenizer {
public:
    JackTokenizer(std::string name) {
        if (name == "")
            ifp = stdin;
        else if ((ifp = fopen(name.c_str(), "r")) == NULL)
            fprintf(stderr, "error: can't open %s for read\n", name.c_str());
    }

    bool hasMoreTokens() { 
        return !feof(ifp);
    }

    int advance() {
        std::string symbol = "{}()[].,;+-*/&|<>=~";
        std::set<std::string> keyword = {"class", "constructor", "function", "method", "field",
            "static", "var", "int", "char", "boolean", "void", "true", "false",
            "null", "this", "let", "do", "if", "else", "while", "return"};

        int c, d;
        char s[100];
        char *p = s;     
        
        while ((c = getc(ifp)) != EOF) {
            if (isspace(c)) {
                continue;
            } else if (c == '/') {
                if ((d = getc(ifp)) == '/') {
                    while ((d = getc(ifp)) != EOF && d != '\n')
                        ;
                } else if (d == '*') {
                    while ((d = getc(ifp)) != EOF) {
                        if (d == '*') {
                            if ((d = getc(ifp)) == '/')
                                break;
                            else
                                ungetc(d, ifp);
                        }
                    }
                } else {
                    ungetc(d, ifp);
                    break;
                }
            } else {
                break;
            }
        }

        if (isalpha(c) || c == '_') {
            for (*p++ = c; isalnum(c = getc(ifp)) || c == '_'; )
                *p++ = c;
            ungetc(c, ifp);
            *p = '\0';
            return tokentype = (keyword.find(tokenvalue = s) != keyword.end()) ? KEYWORD : IDENTIFIER;
        }

        if (isdigit(c)) {
            for (*p++ = c; isdigit(c = getc(ifp));)
                *p++ = c;
            ungetc(c, ifp);
            *p = '\0';
            tokenvalue = s;
            return tokentype = INT_CONST;
        }

        if (c == '\"') {
            for (; (*p = getc(ifp)) != '\"'; p++)
                ;
            *p = '\0';
            tokenvalue = s;
            return tokentype = STRING_CONST;
        }

        *p = c;
        *++p = '\0';
        return tokentype = (symbol.find(tokenvalue = s) != std::string::npos) ? SYMBOL : c;
    }

    int tokenType() {
        return tokentype;
    }

    std::string token() {
        return tokenvalue;
    }

    std::string keyword() {
        return tokenvalue;
    }

    std::string symbol() {
        return tokenvalue;
    }

    std::string identifier() {
        return tokenvalue;
    }

    int intVal() {
        return stoi(tokenvalue);
    }

    std::string stringVal() {
        return tokenvalue;
    }

    ~JackTokenizer() {
        if (ifp != stdin)
            fclose(ifp);
    }

private:
    FILE *ifp;
    int tokentype;
    std::string tokenvalue;
};

#endif
