// #include "assembler.h"
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <regex>
#include <map>
#include <bitset>
#include <algorithm>

using namespace std;

class Instruction {
public:
    string command;
    string reg1, reg2, reg3;
    int r1, r2, r3, reg_num;
    unsigned int shamt :  5;
    unsigned int imm   : 16;
    char type;
    bool has_shamt = false;

    Instruction() {};

    // command reg1, reg2, reg3 R-0
    Instruction(string &&_command, string &&_reg1, string &&_reg2, string &&_reg3, char _type) :
        command(_command), reg1(_reg1), reg2(_reg2), reg3(_reg3), type(_type) {
            reg_num = 3;
        }

    // command reg1, reg2 R-1
    Instruction(string &&_command, string &&_reg1, string &&_reg2, char _type) :
        command(_command), reg1(_reg1), reg2(_reg2), type(_type) {
            reg_num = 2;
        }

    // command reg1 R-2
    Instruction(string &&_command, string &&_reg1, char _type) :
        command(_command), reg1(_reg1), type(_type) {
            reg_num = 1;
        }

    // command reg1, reg2, shamt R-3
    // command reg1, reg2, imm I-0
    // command reg1, reg2, label I-2
    Instruction(string &&_command, string &&_reg1, string &&_reg2, int num, char _type) :
        command(_command), reg1(_reg1), reg2(_reg2), type(_type) { 
            if (type == 'R') {
                shamt = num;
                has_shamt = true;
            } else if ( type == 'I' ) {
                imm = num;
            }
            reg_num = 2;
        }

    // command reg1, label I-3
    // command reg1, imm I-4
    Instruction(string &&_command, string &&_reg1, int label, char _type) :
        command(_command), reg1(_reg1), imm(label), type(_type) {
            reg_num = 1;
        }

    // command reg1, number(reg2) I-5
    Instruction(string &&_command, string &&_reg1, int offset, string &&_reg2, char _type) :
        command(_command), reg1(_reg1), reg2(_reg2), imm(offset), type(_type) {
            reg_num = 2;
        }

    // command label J-0
    Instruction(string &&_command, int label, char _type) :
        command(_command), imm(label), type(_type) { 
            reg_num = 0;
        }

    unsigned long binary();
    
    static map<string, unsigned int> opcode;
    static map<string, unsigned int> func;
    static map<string, unsigned int> regs;
};

map<string, unsigned int> Instruction::opcode{ 
    {"add", 0}, {"addu", 0}, 
    {"and", 0}, {"nor", 0}, 
    {"or", 0}, {"sllv", 0},
    {"srav", 0}, {"srlv", 0},
    {"sub", 0}, {"subu", 0},
    {"xor", 0}, {"slt", 0},
    {"sltu", 0}, {"div", 0},
    {"divu", 0}, {"jalr", 0},
    {"sll", 0}, {"sra", 0},
    {"srl", 0}, {"jr", 0},
    {"mfhi", 0}, {"mflo", 0},
    {"mthi", 0}, {"mtlo", 0},
    {"mul", 0x1c}, {"clo", 0x1c},
    {"clz", 0x1c}, {"madd", 0x1c},
    {"maddu", 0x1c}, {"msub", 0x1c},
    {"msubu", 0x1c}, {"addi", 8},
    {"addiu", 9}, {"addi", 0xc}, 
    {"ori", 0xd}, {"xori", 0xe},
    {"slti", 0xa}, {"sltiu", 0xb},
    {"beq", 4}, {"bne", 5},
    {"lui", 0xf}, {"bgez", 1},
    {"bgezal", 1}, {"bgtz", 7},
    {"blez", 6}, {"bltzal", 1},
    {"bltz", 1}, {"lb", 0x20},
    {"lbu", 0x24}, {"lh", 0x21},
    {"lhu", 0x25}, {"lw", 0x23},
    {"sb", 0x28}, {"sh", 0x29},
    {"sw", 0x2b}, {"j", 2},
    {"jal", 3}, {"andi", 0xc}
};

map<string, unsigned int> Instruction::func = {
    {"add", 0x20}, {"addu", 0x21},
    {"and", 0x24}, {"mul", 2},
    {"nor", 0x27}, {"or", 0x25},
    {"sllv", 4}, {"srav", 7},
    {"sub", 0x22}, {"subu", 0x23},
    {"xor", 0x26}, {"slt", 0x2a},
    {"sltu", 0x2b}, {"clo", 0x21},
    {"clz", 0x20}, {"mult", 0x18},
    {"multu", 0x19}, {"madd", 0},
    {"maddu", 1}, {"msub", 4},
    {"msubu", 5}, {"div", 0x1a},
    {"diu", 0x1b}, {"jalr", 0},
    {"sll", 0}, {"sra", 3},
    {"srl", 2}, {"mfhi", 0x10},
    {"mflo", 0x12}, {"mthi", 0x11},
    {"mtlo", 0x13}, {"jr", 8}
};

map<string, unsigned int> Instruction::regs = {
    {"zero", 0}, {"at", 1},
    {"v0", 2}, {"v1", 3},
    {"a0", 4}, {"a1", 5},
    {"a2", 6}, {"a3", 7},
    {"t0", 8}, {"t1", 9},
    {"t2", 10}, {"t3", 11},
    {"t4", 12}, {"t5", 13},
    {"t6", 14}, {"t7", 15},
    {"s0", 16}, {"s1", 17},
    {"s2", 18}, {"s3", 18},
    {"s4", 20}, {"s5", 21},
    {"s6", 22}, {"s7", 23},
    {"t8", 24}, {"t9", 25},
    {"k0", 26}, {"k1", 27},
    {"gp", 28}, {"sp", 29},
    {"fp", 30}, {"ra", 31}
};

unsigned long Instruction::binary() {
    unsigned long result = 0;

    switch ( type ) {
        case 'R': 
            result += opcode[command] << 26;
            if (reg_num == 1) {
                if ( command == "mfhi" || command == "mflo" ) {
                    result += regs[reg1] << 11;
                } else {
                    result += regs[reg1] << 21;
                }
            } else if ( reg_num == 2 ) {
                if(has_shamt) {
                    result += (regs[reg1] << 11) + (regs[reg2] << 16);
                    result += shamt << 6;
                } else if ( command == "jarl" ) {
                    result += (regs[reg1] << 21) + (regs[reg2] << 11);
                } else if ( command == "clo" || command == "clz" ) {
                    result += (regs[reg1] << 11) + (regs[reg2] << 21);
                } else {
                    result += (regs[reg1] << 21) + (regs[reg2] << 16);
                }
            } else {
                result += (regs[reg1] << 11) + (regs[reg2] << 21) + (regs[reg3] << 16);
            }
            result += func[command];
            break;
        case 'I': 
            result += opcode[command] << 26;
            if (reg_num == 1) {
                if (command == "lui") result += regs[reg1] << 16;
                else result += regs[reg1] << 21;
            }
            if (reg_num ==2) {
                if (command == "beq" || command == "bne") {
                    result += regs[reg2] << 16;
                    result += regs[reg1] << 21;
                } else {
                    result += regs[reg1] << 16;
                    result += regs[reg2] << 21;
                }
            }
            result += imm;
            break;
        case 'J': 
            result += opcode[command];
            result += imm;
            break;
    }

    return result;
}

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

void Assembler::parse() {
    // No white space at the begin and end of instruction
    static vector<regex> types {
        // command reg1, reg2, imm I-0
        regex("(\\w+)\\s*\\$(\\w+)\\s*,\\s*\\$(\\w+)\\s*,\\s*(-?\\d+)"),
        // command reg1, imm I-1
        regex("(\\w+)\\s*\\$(\\w+)\\s*,\\s*(-?\\d+)"),
        // command reg1, reg2, label I-2
        regex("(\\w+)\\s*\\$(\\w+)\\s*,\\s*\\$(\\w+)\\s*,\\s*(\\w+)"),
        // command reg1, label I-3
        regex("(\\w+)\\s*\\$(\\w+)\\s*,\\s*(\\w+)"),
        // command reg1, number(reg2) I-4
        regex("(\\w+)\\s*\\$(\\w+)\\s*,\\s*(-?\\d+)\\s*\\(\\s*\\$(\\w+)\\s*\\)"),
        // command reg1, reg2, reg3 R-0
        regex("(\\w+)\\s*\\$(\\w+)\\s*,\\s*\\$(\\w+)\\s*,\\s*\\$(\\w+)"),
        // command reg1, reg2 R-1
        regex("(\\w+)\\s*\\$(\\w+)\\s*,\\s*\\$(\\w+)"),
        // command reg1 R-2
        regex("(\\w+)\\s*\\$(\\w+)"),
        // command reg1, reg2, shamt R-3
        regex("(\\w+)\\s*\\$(\\w+)\\s*,\\s*\\$(\\w+)\\s*,\\s*(-?\\d+)"),
        // command label J-0
        regex("(\\w+)\\s*(\\w+)")
    };

    // test
    smatch result;
    for(auto i = statements.begin(); i != statements.end(); i++) {
        for(auto j = types.begin(); j != types.end(); j++) 
            if ( regex_match(*i, result, *j) ) {
                // cout << *i << endl;
                // for(size_t k = 1; k < result.size(); k++) cout << result[k].str() << endl;
                Instruction *tmp;
                int imm = 0;
                switch (j-types.begin()) {
                    case 0 : 
                        tmp = new Instruction(result[1].str(), result[2].str(), result[3].str(),
                                atoi(result[4].str().c_str()), 'I');
                        break;
                    case 1 : 
                        tmp = new Instruction(result[1].str(), result[2].str(), 
                                atoi(result[2].str().c_str()), 'I');
                        break;
                    case 2 : 
                        imm = labels[result[4].str()] - (i - statements.begin() + 1) & ((1 << 16) - 1);
                        tmp = new Instruction(result[1].str(), result[2].str(), result[3].str(), imm, 'I');
                        break;
                    case 3 : 
                        imm = labels[result[4].str()] - (i - statements.begin() + 1) & ((1 << 16) - 1);
                        tmp = new Instruction(result[1].str(), result[2].str(), imm, 'I');
                        break;
                    case 4 : 
                        tmp = new Instruction(result[1].str(), result[2].str(), atoi(result[3].str().c_str()),
                               result[4].str(), 'I');
                        break;
                    case 5 : 
                        tmp = new Instruction(result[1].str(), result[2].str(), result[3].str(), 
                                result[4].str(), 'R');
                        break;
                    case 6 : 
                        tmp = new Instruction(result[1].str(), result[2].str(), result[3].str(), 'R');
                        break;
                    case 7 : 
                        tmp = new Instruction(result[1].str(), result[2].str(), 'R');
                        break;
                    case 8 : 
                        tmp = new Instruction(result[1].str(), result[2].str(), 
                                atoi(result[3].str().c_str()), 'R');
                        break;
                    case 9 : 
                        imm = labels[result[2].str()];
                        tmp = new Instruction(result[1].str(), imm, 'I');
                        break;
                }
                // cout << bitset<32>(tmp->binary()) << endl;
                cout << hex << tmp->binary() << endl;
                break;
            } 
    }
}

void Assembler::calc_label() {
    static regex label_regex("(\\w+):");
    size_t i = 0;
    while ( i < statements.size() ) {
        string &str = statements[i];
        if ( regex_match(str, label_regex) ) {
            labels.insert( {str.substr(0, str.length()-1), i} );
            statements.erase( statements.begin() + i );
        } else i++;
    }
}

void Assembler::trim() {
    static regex trim("(^\\s+)|(\\s+$)");
    static regex white("\\s+");
    static regex format("\\s*([,:])\\s*");
    static regex comment("#.*$");
    for(auto i=statements.begin(); i!=statements.end(); i++) {
        // trim space ' '
        // while( ( pos = i->find_first_of(' ') ) != string::npos ) {
        //     i->erase(pos, 1);
        // }

        // // trim tab '\t'
        // while( ( pos = i->find_first_of('\t') ) != string::npos ) {
        //     i->erase(pos, 1);
        // }
        *i = regex_replace(*i, trim, "");
        // *i = regex_replace(*i, white, " ");
        // *i = regex_replace(*i, format, "$1 ");
        *i = regex_replace(*i, comment, "");
    }
}

void Assembler::split() {
    stringstream buffer( content );
    string item;
    while( getline(buffer, item, '\n') ) {
        transform(item.begin(), item.end(), item.begin(), ::tolower);
        if (item.length() > 1) statements.push_back(item);
    }
}

void Assembler::format() {
    static regex label("(\\s*(\\w+)\\s*:\\s*).*");
    // 0. remove empty line
    {
        auto i = statements.begin();
        while( i != statements.end() ) {
            if (i->length() < 1) statements.erase(i);
            else i++;
        }
    }
    // 1. extract label into new line:
    size_t i = 0;
    while ( i < statements.size() ) {
        string &str = statements[i];
        smatch result;
        if ( regex_match(str, result, label) ) {
            // if ( result.size() == 3 ) {
            //     labels.insert( { result[2].str(), i } );
            // }
            if ( (size_t) result[1].length() != str.length() ) {
                // statements.erase(statements.begin()+i);
                string tmp = result[2].str();
                str.erase( str.find(result[1].str()), result[1].length() );
                statements.insert(statements.begin()+i,tmp+":");
                i++;
            }
            i++;
        } else i++;
    }
}

void Assembler::print() {
    for(auto i = statements.begin(); i != statements.end(); i++) {
        cout << i-statements.begin() << " : " << *i << endl;
    }
}
