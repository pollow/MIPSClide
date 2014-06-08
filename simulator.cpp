#include "simulator.h"

using namespace std;

void Simulator::run() {
    while (pc < statements.size()) {
        cout << statements[pc] << endl;
        tick();
    }

    for(auto i : reg_name) {
        cout << i.first << " : " << dec << regs[i.second] << endl;
    }
}

void Simulator::debug() {
    char c;
    int start, length;
    while (pc < statements.size()) {
        cout << statements[pc] << endl;
        c = getchar();
        cout << (int)c << endl;
        if ( c == 'n' ) {
            tick();
        } else if ( c == 's' ) {
            size_t tmp = pc + 1;
            while (pc != tmp) tick();
        } else if ( c == 'r' ) {
            for(auto i : reg_name) {
                cout << i.first << " : " << regs[i.second] << endl;
            }
        } else if ( c == 'm' ) {
            cin >> start >> length;
            for(int i = start; i < start+length; i++) {
                cout << i << ' ' << memory[i];
            }
        } else if ( c == 'e' ) {
            run();
        }
    }
}

bool Simulator::is_number(const std::string& s)
{
    return !s.empty() && std::find_if(s.begin(), 
        s.end(), [](char c) { return !std::isdigit(c); }) == s.end();
}

void Simulator::init() {
    origin.assign(statements.begin(), statements.end());
    split();
    trim();
    format();
    calc_label();
    parse();
    
    memory = new int32_t[300000];
    memset(memory, 0, sizeof(int32_t) * 300000);
    memset(regs, 0, sizeof(int32_t) * 32);
    pc = 0;

    print();
}

void Simulator::tick() {
    pc++;
    string com = elements[pc-1][0];
    string tmp;
    if ( elements[pc-1].size() == 2 ) {
        if (Instruction::regs.count(elements[pc-1][1])) {
            (this->*(func_table[com]))(Instruction::regs[elements[pc-1][1]], 0, 0);
        } else if (is_number(elements[pc-1][1])) {
            (this->*(func_table[com]))(atoi(elements[pc-1][1].c_str()), 0, 0);
        } else {
            (this->*(func_table[com]))(labels[elements[pc-1][1]], 0, 0);
        }
    } else if (elements[pc-1].size() == 3 ) {
        if (Instruction::regs.count(elements[pc-1][2])) {
            (this->*(func_table[com]))(Instruction::regs[elements[pc-1][1]], Instruction::regs[elements[pc-1][2]], 0);
        } else if (is_number(elements[pc-1][2])) {
            (this->*(func_table[com]))(Instruction::regs[elements[pc-1][1]], atoi(elements[pc-1][2].c_str()), 0);
        } else {
            (this->*(func_table[com]))(Instruction::regs[elements[pc-1][1]], labels[elements[pc-1][2]], 0);
        }
    } else {
        if (Instruction::regs.count(elements[pc-1][3])) {
            (this->*(func_table[com]))(Instruction::regs[elements[pc-1][1]], Instruction::regs[elements[pc-1][2]], Instruction::regs[elements[pc-1][3]]);
        } else if (is_number(elements[pc-1][3])) {
            (this->*(func_table[com]))(Instruction::regs[elements[pc-1][1]], Instruction::regs[elements[pc-1][2]], atoi(elements[pc-1][3].c_str()));
        } else {
            (this->*(func_table[com]))(Instruction::regs[elements[pc-1][1]], Instruction::regs[elements[pc-1][2]], labels[elements[pc-1][3]]);
        }
    }
}

void Simulator::_add(int rd, int rs, int rt) {
    regs[rd] = regs[rs] + regs[rt];
}

void Simulator::_sub(int rd, int rs, int rt) {
    regs[rd] = regs[rs] - regs[rt];
}

void Simulator::_and(int rd, int rs, int rt) {
    regs[rd] = regs[rs] & regs[rt];
}

void Simulator::_or(int rd, int rs, int rt) {
    regs[rd] = regs[rs] | regs[rt];
}

void Simulator::_xor(int rd, int rs, int rt) {
    regs[rd] = regs[rs] ^ regs[rt];
}

void Simulator::_nor(int rd, int rs, int rt) {
    regs[rd] = ~(regs[rs] | regs[rt]);
}

void Simulator::_slt(int rd, int rs, int rt) {
    regs[rd] = (regs[rs] < regs[rt]);
}

void Simulator::_srl(int rd, int rt, int shamt) {
    regs[rd] = regs[rt] >> shamt;
}

void Simulator::_sll(int rd, int rt, int shamt) {
    regs[rd] = regs[rt] << shamt;
}

void Simulator::_jr(int rs, int t = 0, int tt = 0) {
    pc = regs[rs];
}

void Simulator::_jalr(int rs, int rd, int t = 0) {
    regs[rd] = pc+1;
    pc = regs[rs];
}

void Simulator::_addi(int rt, int rs, int imm) {
    regs[rt] = regs[rs] + imm;
}

void Simulator::_andi(int rt, int rs, int imm) {
    regs[rt] = regs[rs] & (uint32_t) ((uint16_t)imm);
}

void Simulator::_ori(int rt, int rs, int imm) {
    regs[rt] = regs[rs] | (uint32_t) ((uint16_t)imm);
}

void Simulator::_xori(int rt, int rs, int imm) {
    regs[rt] = regs[rs] ^ (uint32_t) ((uint16_t)imm);
}

void Simulator::_lui(int rt, int imm, int t = 0) {
    regs[rt] = ((uint32_t) ((uint16_t)imm)) << 16;
}

void Simulator::_lw(int rt, int offset, int rs) {
    offset = offset >> 2;
    regs[rt] = memory[offset+(regs[rs] >> 2)];
}

void Simulator::_sw(int rt, int offset, int rs) {
    offset = offset >> 2;
    memory[offset+(regs[rs] >> 2)] = regs[rt];
}

void Simulator::_beq(int rt, int rs, int label) {
    if (regs[rt] == regs[rs]) pc = label - 1;
}

void Simulator::_bne(int rt, int rs, int label) {
    if (regs[rt] != regs[rs]) pc = label - 1;
}

void Simulator::_slti(int rt, int rs, int imm) {
    regs[rt] = (regs[rs] < (uint16_t)imm);
}

void Simulator::_j(int label, int t = 0, int tt = 0) {
    pc = label;
}

void Simulator::_jal(int label, int t = 0, int tt = 0) {
    regs[31] = pc+1;
    pc = label;
}

map<string, unsigned int> Simulator::reg_name = {
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

