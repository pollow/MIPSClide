FLAGS=-g -Wall -DDEBUG -std=c++11

all: ass dis sim

sim: sim.cpp assembler.o instruction.o simulator.o
	c++ sim.cpp simulator.o disassembler.o assembler.o instruction.o -o sim $(FLAGS)

dis: dis.cpp disassembler.o assembler.o instruction.o
	c++ dis.cpp disassembler.o assembler.o instruction.o -o dis $(FLAGS)

ass: ass.cpp disassembler.o assembler.o instruction.o
	c++ ass.cpp disassembler.o assembler.o instruction.o -o ass $(FLAGS)

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
