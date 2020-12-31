all: VMEmulator VMTranslator JackAnalyzer JackCompiler Assembler CPUEmulator 


Assembler: Assembler.cpp ASMParser.h ASMSymbolTable.h Code.h
	g++ -W -Wall -std=c++11 $< -o $@

CPUEmulator: CPUEmulator.cpp ASMParser.h ASMSymbolTable.h
	g++ -W -Wall -std=c++11 $< -o $@

VMTranslator: VMTranslator.cpp CodeWriter.h VMParser.h
	g++ -Wall -std=c++11 $< -o $@

VMEmulator: VMEmulator.cpp VMParser.h
	g++ -Wall -std=c++11 $< -o $@

JackAnalyzer: JackAnalyzer.cpp SymbolTable.h VMWriter.h CompilationEngine.h JackTokenizer.h
	g++ -Wall -std=c++11 $< -o $@

JackCompiler: JackCompiler.cpp SymbolTable.h VMWriter.h CompilationEngine.h JackTokenizer.h
	g++ -Wall -std=c++11 $< -o $@

clean:
	rm -rf VMEmulator VMTranslator JackAnalyzer JackCompiler Assembler CPUEmulator
