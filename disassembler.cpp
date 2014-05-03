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

using namespace std;

class Instruction {
public:
    string command;
    string reg1, reg2, reg3;
    int reg_num;
    int16_t offset;
    unsigned int shamt :  5;
    unsigned int imm   : 16;
    unsigned int target = 0;
    char type;
    bool has_shamt = false, need_label = false, need_target = false;
    unsigned int mcode;

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

    Instruction(unsigned int _mcode) : mcode(_mcode) {}
    unsigned int complie();
    string reverse();
    int getopcode();
    int getfunct();
    int getrs();
    int getrt();
    int getrd();
    int getshamt();
    int getimm();
    int gettarget();
   
    static map<string, unsigned int> opcode;
    static map<string, unsigned int> func;
    static map<string, unsigned int> regs;
    static map<unsigned int, string> re_func_0_0; // rd rs rt
    static map<unsigned int, string> re_func_0_1; // rs rt
    static map<unsigned int, string> re_func_0_2; // rd
    static map<unsigned int, string> re_func_0_3; // rs
    static map<unsigned int, string> re_func_0_shamt; // rd, rt, shamt
    static map<unsigned int, string> re_func_1c_0; // rs, rt
    static map<unsigned int, string> re_func_1c_1; // rd, rs
    static map<unsigned int, string> re_op_i_0; // rs, rt, imm
    static map<unsigned int, string> re_op_i_1; // rs, rt, label
    static map<unsigned int, string> re_op_i_2; // rt, imm(rs)
    static map<unsigned int, string> re_branch; // rs
    static map<unsigned int, string> re_jump; // j-type
    static map<unsigned int, string> re_regs;
};

map<unsigned int, string> Instruction::re_jump = {
    {2, "j"}, {3, "jal"}
};

map<unsigned int, string> Instruction::re_branch  = {
    {0x01, "bgez"},     {0x11, "bgezal"},
    {0x10, "bltzal"},   {0x00, "bltz"}
};

map<unsigned int, string> Instruction::re_op_i_0 = {
    {0x08, "addi"},     {0x09, "addiu"},
    {0x0c, "addi"},     {0x0d, "ori"},
    {0x0e, "xori"},     {0x0a, "slti"},
    {0x0b, "sltiu"}
};

map<unsigned int, string> Instruction::re_op_i_1 = {
    {0x04, "beq"}, {0x05, "bne"}
};

map<unsigned int, string> Instruction::re_op_i_2 = {
    {0x20, "lb"},       {0x24, "lbu"},
    {0x21, "lh"},       {0x25, "lhu"},
    {0x23, "lw"},       {0x28, "sb"},
    {0x29, "sh"},       {0x2b, "sw"}
};

map<string, unsigned int> Instruction::opcode{
    {"add", 0},         {"addu", 0},
    {"and", 0},         {"nor", 0},
    {"or", 0},          {"sllv", 0},
    {"srav", 0},        {"srlv", 0},
    {"sub", 0},         {"subu", 0},
    {"xor", 0},         {"slt", 0},
    {"sltu", 0},        {"div", 0},
    {"divu", 0},        {"jalr", 0},
    {"sll", 0},         {"sra", 0},
    {"srl", 0},         {"jr", 0},
    {"mfhi", 0},        {"mflo", 0},
    {"mthi", 0},        {"mtlo", 0},
    {"mul", 0x1c},      {"clo", 0x1c},
    {"clz", 0x1c},      {"madd", 0x1c},
    {"maddu", 0x1c},    {"msub", 0x1c},
    {"msubu", 0x1c},    {"addi", 8},
    {"addiu", 9},       {"addi", 0xc},
    {"ori", 0xd},       {"xori", 0xe},
    {"slti", 0xa},      {"sltiu", 0xb},
    {"beq", 4},         {"bne", 5},
    {"lui", 0xf},       {"bgez", 1},
    {"bgezal", 1},      {"bgtz", 7},
    {"blez", 6},        {"bltzal", 1},
    {"bltz", 1},        {"lb", 0x20},
    {"lbu", 0x24},      {"lh", 0x21},
    {"lhu", 0x25},      {"lw", 0x23},
    {"sb", 0x28},       {"sh", 0x29},
    {"sw", 0x2b},       {"j", 2},
    {"jal", 3},         {"andi", 0xc}
};

// rd rs rt
map<unsigned int, string> Instruction::re_func_0_0 = {
    {0x20, "add"},  {0x21, "addu"},
    {0x24, "and"},  {0x27, "nor"},
    {0x25, "or"},   {0x04, "sllv"},
    {0x07, "srav"}, {0x06, "srlv"},
    {0x22, "sub"},  {0x23, "subu"},
    {0x26, "xor"},  {0x2a, "slt"},
    {0x2b, "sltu"}
};

// rs rt
map<unsigned int, string> Instruction::re_func_0_1 = {
    {0x18, "mult"}, {0x19, "multu"},
    {0x1a, "div"},  {0x1b, "divu"}
};

// rd
map<unsigned int, string> Instruction::re_func_0_2 = {
    {0x10, "mfhi"}, {0x12, "mflo"}
};

// rs
map<unsigned int, string> Instruction::re_func_0_3 = {
    {0x11, "mthi"}, {0x13, "mtlo"},
    {0x08, "jr"}
};

map<unsigned int, string> Instruction::re_func_0_shamt = {
    {0x00, "sll"}, {0x02, "sra"}, {0x03, "sra"}
};

// rs, rt
map<unsigned int, string> Instruction::re_func_1c_0 = {
    {0x00, "madd"}, {0x01, "maddu"},
    {0x04, "msub"}, {0x05, "msubu"}
};

// rd, rs
map<unsigned int, string> Instruction::re_func_1c_1 = {
    {0x21, "clo"}, {0x20, "clz"}
};

map<string, unsigned int> Instruction::func = {
    {"add", 0x20},      {"addu", 0x21},
    {"and", 0x24},      {"mul", 2},
    {"nor", 0x27},      {"or", 0x25},
    {"sllv", 4},        {"srav", 7},
    {"sub", 0x22},      {"subu", 0x23},
    {"xor", 0x26},      {"slt", 0x2a},
    {"sltu", 0x2b},     {"clo", 0x21},
    {"clz", 0x20},      {"mult", 0x18},
    {"multu", 0x19},    {"madd", 0},
    {"maddu", 1},       {"msub", 4},
    {"msubu", 5},       {"div", 0x1a},
    {"diu", 0x1b},      {"jalr", 0},
    {"sll", 0},         {"sra", 3},
    {"srl", 2},         {"mfhi", 0x10},
    {"mflo", 0x12},     {"mthi", 0x11},
    {"mtlo", 0x13},     {"jr", 8}
};

map<string, unsigned int> Instruction::regs = {
    {"zero", 0}, {"at", 1},
    {"v0", 2},   {"v1", 3},
    {"a0", 4},   {"a1", 5},
    {"a2", 6},   {"a3", 7},
    {"t0", 8},   {"t1", 9},
    {"t2", 10},  {"t3", 11},
    {"t4", 12},  {"t5", 13},
    {"t6", 14},  {"t7", 15},
    {"s0", 16},  {"s1", 17},
    {"s2", 18},  {"s3", 18},
    {"s4", 20},  {"s5", 21},
    {"s6", 22},  {"s7", 23},
    {"t8", 24},  {"t9", 25},
    {"k0", 26},  {"k1", 27},
    {"gp", 28},  {"sp", 29},
    {"fp", 30},  {"ra", 31}
};

map<unsigned int, string> Instruction::re_regs = {
    {0, "zero"}, {1, "at"},
    {2, "v0"},   {3, "v1"},
    {4, "a0"},   {5, "a1"},
    {6, "a2"},   {7, "a3"},
    {8, "t0"},   {9, "t1"},
    {10, "t2"},  {11, "t3"},
    {12, "t4"},  {13, "t5"},
    {14, "t6"},  {15, "t7"},
    {16, "s0"},  {17, "s1"},
    {18, "s2"},  {19, "s3"},
    {20, "s4"},  {21, "s5"},
    {22, "s6"},  {23, "s7"},
    {24, "t8"},  {25, "t9"},
    {26, "k0"},  {27, "k1"},
    {28, "gp"},  {29, "sp"},
    {30, "fp"},  {31, "ra"}
};

unsigned int Instruction::complie() {
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

int Instruction::getimm() {
    return mcode & ((1 << 16) - 1);
}

int Instruction::getopcode() {
    return (mcode >> 26) & ((1 << 6) - 1);
}

int Instruction::getfunct() {
    return mcode & ( (1 << 6) - 1 );
}

int Instruction::getrd() {
    return (mcode >> 11) & ((1 << 5) - 1);
}

int Instruction::getrs() {
    return (mcode >> 21) & ((1 << 5) - 1);
}

int Instruction::getrt() {
    return (mcode >> 16) & ((1 << 5) - 1);
}

int Instruction::gettarget() {
    return mcode & ((1 << 23) - 1);
}

string Instruction::reverse() {
    unsigned int op = getopcode();
    unsigned int funct = getfunct();
    unsigned int rs = getrs();
    unsigned int rd = getrd();
    unsigned int rt = getrt();
    signed int imm = getimm();

    string result;

    if ( op == 0 ) {
        if (funct == 0) {
            result = "jalr $" +re_regs[rs] + ", $" + re_regs[rd];
        } else if (re_func_0_0.count(funct)) {
            result = re_func_0_0[funct] + " $" + re_regs[rd] + ", $" + re_regs[rs] + ", $" + re_regs[rt];
        } else if (re_func_0_1.count(funct)) {
            result = re_func_0_1[funct] + " $" + re_regs[rs] + ", $" + re_regs[rt];
        } else if (re_func_0_2.count(funct)) {
            result = re_func_0_2[funct] + " $" + re_regs[rd];
        } else if (re_func_0_3.count(funct)) {
            result = re_func_0_3[funct] + " $" + re_regs[rs];
        } else if (re_func_0_shamt.count(funct)) {
            result = re_func_0_shamt[funct] + " $" + re_regs[rd] + ", $" + re_regs[rs] + ", " + to_string(shamt);
        }
    } else if ( op == 0x1c ) {
        if ( re_func_1c_0.count(funct) ) {
            result = re_func_1c_0[funct] + " $" + re_regs[rs] + ", $" + re_regs[rt];
        } else if ( re_func_1c_1.count(funct) ) {
            result = re_func_1c_0[funct] + " $" + re_regs[rd] + ", $" + re_regs[rs];
        } else {
            result = "mul $" + re_regs[rd] + ", $" + re_regs[rs] + ", $" + re_regs[rt];
        }
    } else if ( op == 0x01 ) {
        need_label = true;
        offset = imm;
        result = re_branch[rt] + " $" + re_regs[rs] + ", ";
    } else {
        if (re_op_i_0.count(op)) {
            result = re_op_i_0[op] + " $" + re_regs[rt] + ", $" + re_regs[rs] + ", " + to_string(imm);
        } else if (re_op_i_1.count(op)) {
            need_label = true;
            offset = imm;
            result = re_op_i_1[op] + " $" + re_regs[rt] + ", $" + re_regs[rs] + ", ";
        } else if (re_op_i_2.count(op)) {
            result = re_op_i_2[op] + " $" + re_regs[rt] + ", " + to_string(imm) + "($" + re_regs[rs] + ")";
        } else if (re_jump.count(op)) {
            need_target = true;
            target = gettarget();
            result = re_jump[op] + " ";
        }
    }

    return result;
}

class Disassembler {
public:
    int pc = 0;

    vector<unsigned int> codes;
    vector<string> instructions;
    map<int, string> labels;

    Disassembler(vector<unsigned int> &a) : codes(a) {}
   
    void reverse();
    void print();
};

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

void Disassembler::print() {
    for(auto i = instructions.begin(); i != instructions.end(); i++) {
        cout << *i << endl;
    }
}

int main() {
    ifstream fin("test.bin", ios::in);
    string buf;
    vector<unsigned int> a;
    unsigned int tmp;
    while(!fin.eof()) {
        fin >> tmp;
        a.push_back(tmp);
    }
    Disassembler D(a);
    D.reverse();
    D.print();

    return 0;
}
