/*
    10  Compiler 0: Syntax Analyzer

    10.1 Background
    10.1.1 Lexical Analysis
        C code
        while (count <= 100) // some loop
            count++;
            // Body of while continues
            ...

        Tokens
        while
        (
        count
        <=
        100
        )
        count
        ++
        ;
        ...

    10.1.2 Grammars
        From a declarative standpoint, the grammer specifies allowable ways to combine tokens,
    also called terminals, into higher-level syntactic elements, also called non-teminals. From 
    an analytic standpoint, the grammar is a prescriptin for doing the reverse: parsing a given
    input (set of tokens resulting from the tokenizing phase) into non-terminals, lower-level
    non-terminals, and eventually teminals that cannot be decomposed any further.
        while (expression) {
            statement;
            statement;
            while (expression) {
                while (expression)
                    statement;
                statement;
            }
        }

    10.1.3 Parsing
        see parsetree.jpg
    
    Recursive Descent Parsing
        There are several algorithms for constructing parse trees.
    The top-down approach, alse called recursive descent parsing, attempts to parse the
    tokens stream recursively, using the nested structure prescribed by the language
    grammar.

    LL(1) Grammars
        This observation can be generalized as follows: whenever a non-terminal has several
    alternative derivation rules, the first token suffices to resolve without ambiguity which
    rule to use. Grammars that have the property are called LL(1). 

    10.2 Specification

    10.2.1 The Jack Language Grammar
        --------------------------------------------------------------------------------------------------
        Lexical elements:   The Jack language includes five types of terminal elements(tokens):
                 keyword:   'class' | 'constructor' | 'function' |
                            'method' | 'field' | 'static' | 'var' |
                            'int' | 'char' | 'boolean' | 'void' | 'true' |
                            'false' | 'null' | 'this' | 'let' | 'do' |
                            'if' | 'else' | 'while' | 'return'
                  symbol:   '{' | '}' | '(' | ')' | '[' | ']' | '.' |
                            ',' | ';' | '+' | '-' | '*' | '/' | '&' |
                            '|' | '<' | '>' | '=' | '~'
         integerConstant:   A decimal number in the range 0 .. 32767
          StringConstant:   '"' A sequence of Unicode characters not includeing double quote or newline '"'
              identifier:   A sequence of letters, digits, and undescore('_') not starting with a digit
        ---------------------------------------------------------------------------------------------------
        Program structuer:  A Jack program is a collection of classes, each appearing in a separate file.
                            The compilation unit is a class. A class is a sequence of tokens structured
                            according to the following context free syntax:
                   class:   'class' className '{' classVarDec* subroutineDec* '}'
             classVarDec:   ('static' | 'field') type varName (',' varName)* ';'
                    type:   'int' | 'char' | 'boolean' | className
           subroutineDec:   ('constructor' | 'function' | 'method') 
                            ('void' | type) subroutineName '(' parameterList ')'
                            subroutineBody
           parameterList:   ((type varName)) (',' typeName)*)?
          subroutineBody:   '{' varDec* statements '}'
                  varDec:   'var' type varName (',' varName)* ';'
               className:   identifier
          subroutineName:   identifier
                 varName:   identifier
        ---------------------------------------------------------------------------------------------------
              Statements: 
              statements:   statement*
               statement:   letStatement | ifStatement | whileStatement |
                             doStatement | returnStatement
            letStatement:   'let' varName ('[' expression ']')? '=' expression ';'
             ifStatement:   'if' '(' expression ')' '{' statements '}'
                            ('else' '{' expression '}')?
          whileStatement:   'while' '(' expression ')' '{' statements '}'
             doStatement:   'do' subroutineCall ';'
         returnStatement:   'return' expression? ';'
        ---------------------------------------------------------------------------------------------------
              Expression:
              expression:   term (op term)*
                    term:   interConstant | stringConstant | keywordConstant |
                            varName | varName '[' expression ']' | subroutineCall |
                            '(' expression ')' | unaryOp term
          subroutineCall:   subroutineName '(' expressionList ')' | (clasName | varName)
                            '.' subroutineName '(' expressoinList ')'
          expressionList:   (expression (',' expression)*)?
                      op:   '+' | '-' | '*' | '/' | '&' | '|' | '<' | '>' | '='
                 unaryOp:   '-' | '~'
         KeywordConstant:   'true' | 'false' | 'null' | 'this'
        ---------------------------------------------------------------------------------------------------
                                Figure 10.5 Complete grammar of the Jack language

    10.2.2 A Syntax Analyzer for the Jack Language
    
    10.2.3 The Syntax Analyzer's Input
        prompt> JackAnalyzer source
    Where source is either a file name of the form Xxx.jack or a directory name
    
    10.2.4 The Syntax Analyzer's Output

    Terminals
        <xxx> terminal </xxx>

        Where xxx is one of the five lexical elements of the Jack language: namely, 
        keyword, symbol, integerConstant, stringConstant, or identifier

    Non-Terminals
        <xxx>
            Recursive code for the body of the xxx element.
        </xxx>
            
        Where xxx is one of the following non-terminals of the Jack grammar:
        * class, classVarDec, subroutineDec, parameterList,
        subroutineBody, varDec;
        * statements, whileStatement, ifStatement, returnStatement, letStatement, doStatement;
        * expression, term, expressionList.
        

    10.3 Implementation
        * JackAnalyzer
        * JackTokenizer
        * CompilationEngine
    10.3.1 The JackAnalyzer Module
    10.3.2 The JackTokenizer Module
    10.3.3 The CompilationEngine Module

    10.4 Perspective

    10.5 Project

    Test Programs
        * Square
        * ExpressionlessSquare
        * ArrayTest

        State 1: Tokenizer
            XxxT.xml

        Stage 2: Parser
            Xxx.xml
    
*/
