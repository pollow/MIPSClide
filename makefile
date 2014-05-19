FLAGS=-g -Wall -std=c++11

all: ass dis

dis: dis.cpp simulator.o disassembler.o assembler.o instruction.o
	c++ dis.cpp simulator.o disassembler.o assembler.o instruction.o -o dis $(FLAGS)

ass: ass.cpp simulator.o disassembler.o assembler.o instruction.o
	c++ ass.cpp simulator.o disassembler.o assembler.o instruction.o -o ass $(FLAGS)

simulator.o: simulator.cpp 
	c++ -c simulator.cpp $(FLAGS)

assembler.o: assembler.cpp 
	c++ -c assembler.cpp $(FLAGS)

disassembler.o: disassembler.cpp 
	c++ -c disassembler.cpp $(FLAGS)

instruction.o: instruction.cpp
	c++ -c instruction.cpp $(FLAGS)

clean:
	rm -rf *.o dis ass *.dSYM
