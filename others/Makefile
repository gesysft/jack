all: VMEmulator  CPUEmulator

CPUEmulator: CPUEmulator.cpp ../ASMParser.h ../ASMSymbolTable.h ../Code.h
	g++ -Wall -std=c++11 $< -o $@

VMEmulator: VMEmulator.cpp ../VMParser.h
	g++ -Wall -std=c++11 $< -o $@

clean:
	rm -rf VMEmulator  CPUEmulator
