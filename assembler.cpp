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
