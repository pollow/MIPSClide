FLAGS=-g -Wall -std=c++11

main: main.cpp assembler.o disassembler.o instruction.o
	c++ main.cpp assembler.o disassembler.o instruction.o -o main $(FLAGS)

assembler.o: assembler.cpp 
	c++ -c assembler.cpp $(FLAGS)

disassembler.o: disassembler.cpp
	c++ -c disassembler.cpp $(FLAGS)

instruction.o: instruction.cpp
	c++ -c instruction.cpp $(FLAGS)

clean:
	rm instruction.o assembler.o disassembler.o main
