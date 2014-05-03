#include <iostream>
#include "assembler.h"
#include "disassembler.h"

using namespace std;

int main(int argc, char *argv[]) {
    fstream fin("test.mips", ios::in);
    string ins;
    getline(fin, ins);
    Assembler A(ins);
    A.trim();
    A.split();
    A.parse();
    A.print();

    return 0;
}
