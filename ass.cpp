#include <iostream>
#include "assembler.h"
#include "disassembler.h"

using namespace std;

int main(int argc, char *argv[]) {
    fstream fin("test.mips", ios::in);
    string ins, content;
    while(!fin.eof()) {
        getline(fin, ins);
        content += ins + "\n";
    }
    Assembler A(content);
    A.split();
    A.trim();
    A.format();
    A.calc_label();
    A.parse();
    A.print();

    return 0;
}
