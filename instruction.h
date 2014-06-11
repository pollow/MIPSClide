#ifndef __INSTRUCTION_H__
#define __INSTRUCTION_H__

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
            if ( command == "sll" || command == "sra" || command == "srl" ) {
                // special case for shamt
                type = 'R';
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
    unsigned int compile();
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

#endif
