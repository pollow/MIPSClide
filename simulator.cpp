#include "instruction.h"
#include "assembler.h"

using namespace std;

class Simulator : public Assembler{
public:
    size_t pc;
    vector<string> origin;
    int32_t *memory, regs[32];

    Simulator( string &_content ) : Assembler(_content) {
        func_table["add"]   = &Simulator::_add ;
        func_table["sub"]   = &Simulator::_sub ;
        func_table["and"]   = &Simulator::_and ;
        func_table["or"]    = &Simulator::_or  ;
        func_table["xor"]   = &Simulator::_xor ;
        func_table["nor"]   = &Simulator::_nor ;
        func_table["slt"]   = &Simulator::_slt ;
        func_table["srl"]   = &Simulator::_srl ;
        func_table["sll"]   = &Simulator::_sll ;
        func_table["jr"]    = &Simulator::_jr  ;
        func_table["jalr"]  = &Simulator::_jalr;
        func_table["addi"]  = &Simulator::_addi;
        func_table["andi"]  = &Simulator::_andi;
        func_table["ori"]   = &Simulator::_ori ;
        func_table["xori"]  = &Simulator::_xori;
        func_table["lui"]   = &Simulator::_lui ;
        func_table["lw"]    = &Simulator::_lw  ;
        func_table["sw"]    = &Simulator::_sw  ;
        func_table["beq"]   = &Simulator::_beq ;
        func_table["bne"]   = &Simulator::_bne ;
        func_table["slti"]  = &Simulator::_slti;
        func_table["j"]     = &Simulator::_j   ;
        func_table["jal"]   = &Simulator::_jal ;
    }

    void init();
    void tick();
    void run();
    void debug();
    void _add (int, int, int);
    void _sub (int, int, int);
    void _and (int, int, int);
    void _or  (int, int, int);
    void _xor (int, int, int);
    void _nor (int, int, int);
    void _slt (int, int, int);
    void _srl (int, int, int);
    void _sll (int, int, int);
    void _jr  (int, int, int);
    void _jalr(int, int, int);
    void _addi(int, int, int);
    void _andi(int, int, int);
    void _ori (int, int, int);
    void _xori(int, int, int);
    void _lui (int, int, int);
    void _lw  (int, int, int);
    void _sw  (int, int, int);
    void _beq (int, int, int);
    void _bne (int, int, int);
    void _slti(int, int, int);
    void _j   (int, int, int);
    void _jal (int, int, int);

    static bool is_number(const std::string& s);

    map<string, void (Simulator::*)(int, int, int)> func_table;
};

void Simulator::run() {
    while (pc < statements.size()) {
        cout << statements[pc] << endl;
        tick();
        pc++;
    }
}

void Simulator::debug() {
    string c;
    int start, length;
    while (pc < statements.size()) {
        cin >> c;
        if ( c == "n" ) {
            tick();
            cout << statements[pc] << endl;
        } else if ( c == "s" ) {
            size_t tmp = pc + 1;
            while (pc != tmp) tick();
            cout << statements[pc] << endl;
        } else if ( c == "r" ) {
            for(auto i : Instruction::regs) {
                cout << i.first << " : " << regs[i.second] << endl;
            }
        } else if ( c == "m" ) {
            cin >> start >> length;
            for(int i = start; i < start+length; i++) {
                cout << i << ' ' << memory[i];
            }
        }
        pc++;
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

    print();
}

void Simulator::tick() {
    string com = elements[pc][0];
    string tmp;
    if ( elements[pc].size() == 2 ) {
        if (Instruction::regs.count(elements[pc][1])) {
            (this->*(func_table[com]))(Instruction::regs[elements[pc][1]], 0, 0);
        } else if (is_number(elements[pc][1])) {
            (this->*(func_table[com]))(atoi(elements[pc][1].c_str()), 0, 0);
        } else {
            (this->*(func_table[com]))(labels[elements[pc][1]], 0, 0);
        }
    } else if (elements[pc].size() == 3 ) {
        if (Instruction::regs.count(elements[pc][2])) {
            (this->*(func_table[com]))(Instruction::regs[elements[pc][1]], Instruction::regs[elements[pc][2]], 0);
        } else if (is_number(elements[pc][2])) {
            (this->*(func_table[com]))(Instruction::regs[elements[pc][1]], atoi(elements[pc][2].c_str()), 0);
        } else {
            (this->*(func_table[com]))(Instruction::regs[elements[pc][1]], labels[elements[pc][2]], 0);
        }
    } else {
        if (Instruction::regs.count(elements[pc][3])) {
            (this->*(func_table[com]))(Instruction::regs[elements[pc][1]], Instruction::regs[elements[pc][2]], Instruction::regs[elements[pc][3]]);
        } else if (is_number(elements[pc][3])) {
            (this->*(func_table[com]))(Instruction::regs[elements[pc][1]], Instruction::regs[elements[pc][2]], atoi(elements[pc][3].c_str()));
        } else {
            (this->*(func_table[com]))(Instruction::regs[elements[pc][1]], Instruction::regs[elements[pc][2]], labels[elements[pc][3]]);
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

