all: Assembler VMTranslator JackAnalyzer JackCompiler 

Assembler: Assembler.cpp ASMParser.h ASMSymbolTable.h Code.h
	g++ -Wall -std=c++11 $< -o $@

VMTranslator: VMTranslator.cpp CodeWriter.h VMParser.h
	g++ -Wall -std=c++11 $< -o $@

JackAnalyzer: JackAnalyzer.cpp SymbolTable.h VMWriter.h CompilationEngine.h JackTokenizer.h
	g++ -Wall -std=c++11 $< -o $@

JackCompiler: JackCompiler.cpp SymbolTable.h VMWriter.h CompilationEngine.h JackTokenizer.h
	g++ -Wall -std=c++11 $< -o $@

clean:
	rm -rf Assembler VMTranslator JackAnalyzer JackCompiler 
