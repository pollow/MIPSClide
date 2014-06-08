#include "disassembler.h"

using namespace std;

void Disassembler::reverse() {
    string str;
    for(size_t i = 0; i < codes.size(); i++) {
        Instruction tmp(codes[i]);
        str = tmp.reverse();
        if ( tmp.need_label ) {
            unsigned int t = pc+tmp.offset+1;
            if (labels.count(t) == 0) {
                labels[t] = "Label"+to_string(labels.size());
            }
            str += "Label"+to_string(labels.size());
        } else if ( tmp.need_target ) {
            unsigned int t = tmp.target;
            if (labels.count(t) == 0) {
                labels[t] = "Label"+to_string(labels.size());
            }
            str += "Label"+to_string(labels.size());
        }
        instructions.push_back("\t" + str);
        pc++;
    }
    for(auto i = labels.rbegin(); i != labels.rend(); i++) {
        instructions.insert(instructions.begin() + i->first, i->second + ":");
    }
}

void Disassembler::print(ostream &out) {
    for(auto i = instructions.begin(); i != instructions.end(); i++) {
        out << *i << endl;
    }
}
