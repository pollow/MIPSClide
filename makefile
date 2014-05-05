FLAGS=-g -Wall -std=c++11

main: simulator.cpp assembler.o instruction.o
	c++ simulator.cpp assembler.o instruction.o -o main $(FLAGS)

assembler.o: assembler.cpp 
	c++ -c assembler.cpp $(FLAGS)

instruction.o: instruction.cpp
	c++ -c instruction.cpp $(FLAGS)

clean:
	rm instruction.o assembler.o main
