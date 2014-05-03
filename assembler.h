#ifndef __ASSEMBLER__H__
#define __ASSEMBLER__H__

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <regex>
#include <map>
#include <string>
#include <bitset>
#include <algorithm>
#include "instruction.h"

class Assembler {
public:
    string content;
    vector<string> statements;
    vector<vector<string>> elements;
    map<string, int> labels;

    Assembler(string &_content) : content(_content) {}
    void split();
    void trim();
    void format();
    void calc_label();
    void parse();
    void print();
};

#endif
