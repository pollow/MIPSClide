#include <iostream>
#include "assembler.h"
#include "disassembler.h"

using namespace std;

int main(int argc, char *argv[]) {
    fstream fin("test.bin", ios::in);
    unsigned int a = 0;
    vector<unsigned int> inst;
    while(fin >> a) {
        inst.push_back(a);
    }
    Disassembler A(inst);
    A.reverse();
    A.print();

    return 0;
}
