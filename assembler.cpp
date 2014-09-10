#include "assembler.h"

using namespace std;

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
        // command reg1, reg2, shamt R-3 useless, same as the first.
        regex("(\\w+)\\s*\\$(\\w+)\\s*,\\s*\\$(\\w+)\\s*,\\s*(-?\\d+)"),
        // command label J-0
        regex("(\\w+)\\s*(\\w+)")
    };

    smatch result;
    for(auto i = statements.begin(); i != statements.end(); i++) {
        try {
            static regex comma_split("(^\\s+)|(\\s+$)");
            if ( tolower((*i)[0]) == 'd' && tolower((*i)[1]) == 'd' ) {
                string data = i->substr(3, i->size()-3);
                data = regex_replace(data, comma_split, " ");
                stringstream buffer( data );
                buffer >> setbase(0);
                for(unsigned int i; buffer >> i; ) {
                    machine_code.push_back(i);
                }
            } else if ( i->substr(0, 4) == "resd" || i->substr(0, 4) == "RESD" ) {
                string data = i->substr(4, i->size()-4);
                stringstream buffer( data );
                int count;
                buffer >> setbase(0) >> count;
                for(int i = 0; i < count; i++) machine_code.push_back(0);
            } else {
                auto j = types.begin();
                for(; j != types.end(); j++) 
                    if ( regex_match(*i, result, *j) ) {
                        // cout << *i << endl;
                        elements.push_back( {} );
                        for(size_t k = 1; k < result.size(); k++) 
                            elements.back().push_back(result[k].str());
                        Instruction *tmp;
                        int imm = 0;
                        switch (j-types.begin()) {
                            case 0 : 
                                tmp = new Instruction(result[1].str(), result[2].str(), result[3].str(),
                                        atoi(result[4].str().c_str()), 'I');
                                break;
                            case 1 : 
                                tmp = new Instruction(result[1].str(), result[2].str(), 
                                        atoi(result[3].str().c_str()), 'I');
                                break;
                            case 2 : 
                                if (!labels.count(result[4].str())) {
                                    throw invalid_argument("Label undefined");
                                }
                                imm = labels[result[4].str()] - (i - statements.begin()+1) & ((1 << 16) - 1);
                                tmp = new Instruction(result[1].str(), result[2].str(), result[3].str(), imm, 'I');
                                break;
                            case 3 : 
                                if (!labels.count(result[4].str())) {
                                    throw invalid_argument("Label undefined");
                                }
                                imm = labels[result[4].str()] - (i - statements.begin()+1) & ((1 << 16) - 1);
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
                                if (!labels.count(result[2].str())) {
                                    throw invalid_argument("Label undefined");
                                }
                                imm = labels[result[2].str()];
                                tmp = new Instruction(result[1].str(), imm, 'I');
                                break;
                        }
                        machine_code.push_back(tmp->compile());
                        break;
                    }
                if ( j == types.end() ) {
                    throw invalid_argument("Syntax Error");
                }
            }
        } catch(exception &err) {
            throw invalid_argument("Error occurs when parsing:\n\t" + string(err.what()) + "\n\t\tat " + (*i));
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
    static regex comment("\\s*#.*");
    for(auto i=statements.begin(); i!=statements.end(); i++) {
        *i = regex_replace(*i, trim, "");
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
            if ( (size_t) result[1].length() != str.length() ) {
                string tmp = result[2].str();
                str.erase( str.find(result[1].str()), result[1].length() );
                statements.insert(statements.begin()+i,tmp+":");
                i++;
            }
            i++;
        } else i++;
    }
}

void Assembler::print(string type, ostream &out) {

#ifdef DEBUG
    for(auto i = statements.begin(); i != statements.end(); i++) {
        cout << setfill('0') << setw(4) << i-statements.begin() << " : " << *i << endl;
    }
#endif

    if ( type == "coe" ) {
        out << "memory_initialization_radix=16;" << endl << "memory_initialization_vector=" << endl;
        for(auto i : machine_code) {
            out << setfill('0') << setw(8) << hex << i << ',' << endl;
        }
        out << "00000000;" << endl;
    } else if ( type == "plain" ) {
        for(auto i : machine_code) {
            out << setfill('0') << setw(8) << hex << i << endl;
        }
    } else {
        throw invalid_argument("Output type could only be 'coe' or 'plain'");
    }
}

unsigned int Assembler::reverse_word(unsigned int a) {
    return ((0xFF & a) << 24) | ((0xFF00 & a) << 8) | ((0xFF0000 & a) >> 8) | ((0xFF000000 & a) >> 24);
}
