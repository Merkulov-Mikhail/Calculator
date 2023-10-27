COMPILER=g+++ -O0 -g -Wno-pointer-arith


.PHONY: all

all: assembly.exe disassembly.exe processor.exe


assembly.exe: build/assembly.o
	$(COMPILER) $< -o $@

disassembly.exe: build/disassembly.o
	$(COMPILER) $< -o $@

processor.exe: build/processor.o build/stk.a
	$(COMPILER) $^ -o $@

build/%.o: build/src/%.cpp build/src/config.h
	$(COMPILER) -c $< -o $@
