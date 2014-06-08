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
        if ( tolower((*i)[0]) == 'd' || (*i)[1] == 'D' ) {
            static regex comma_split("(^\\s+)|(\\s+$)");
            string data = i->substr(3, i->size()-3);
            data = regex_replace(data, comma_split, " ");
            stringstream buffer( content );
            // convert into number
        } else if ( i->substr(0, 4) == "resd" || i->substr(0, 4) == "RESD" ) {
            // convert into number
        } else for(auto j = types.begin(); j != types.end(); j++) 
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
                                atoi(result[2].str().c_str()), 'I');
                        break;
                    case 2 : 
                        imm = labels[result[4].str()] - (i - statements.begin()) & ((1 << 16) - 1);
                        tmp = new Instruction(result[1].str(), result[2].str(), result[3].str(), imm, 'I');
                        break;
                    case 3 : 
                        imm = labels[result[4].str()] - (i - statements.begin()) & ((1 << 16) - 1);
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
                machine_code.push_back(tmp->complie());
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
    static regex comment("#.*$");
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

void Assembler::print() {
#ifdef DEBUG
    for(auto i = statements.begin(); i != statements.end(); i++) {
        cout << setfill('0') << setw(4) << i-statements.begin() << " : " << *i << endl;
    }
#endif

    for(auto i = machine_code.begin(); i != machine_code.end(); i++) {
        cout << "0x" << setfill('0') << setw(8) << hex << *i << endl;
    }
}

unsigned int Assembler::reverse_word(unsigned int a) {
    return ((0xFF & a) << 24) | ((0xFF00 & a) << 8) | ((0xFF0000 & a) >> 8) | ((0xFF000000 * a) >> 24);
}
