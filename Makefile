COMPILER=g++


.PHONY: all

all: assembly.exe disassembly.exe processor.exe


assembly.exe: build/assembly.o
	$(COMPILER) $< -o $@

disassembly.exe: build/disassembly.o
	$(COMPILER) $< -o $@

processor.exe: build/processor.o
	$(COMPILER) $< -o $@


build/processor.o: build/src/processor.cpp build/src/config.h
	$(COMPILER) -c $< -o $@

build/assembly.o: build/src/assembly.cpp build/src/config.h
	$(COMPILER) -c $< -o $@

build/disassembly.o: build/src/disassembly.cpp build/src/config.h
	$(COMPILER) -c $< -o $@
