#ifndef __DISASSEMBLER_H__
#define __DISASSEMBLER_H__

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <regex>
#include <map>
#include <bitset>
#include <cstdint>
#include <algorithm>
#include "instruction.h"

using namespace std;

class Disassembler {
public:
    int pc = 0;

    vector<unsigned int> codes;
    vector<string> instructions;
    map<int, string> labels;

    Disassembler(vector<unsigned int> &a) : codes(a) {}
   
    void reverse();
    void print(ostream &);
};

#endif
