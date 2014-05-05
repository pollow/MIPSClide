
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
