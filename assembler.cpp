// #include "assembler.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <regex>
#include <map>
#include <algorithm>

using namespace std;

class Assembler {
public:
    string content;
    vector<string> statements;
    map<string, int> labels;

    Assembler(string &_content) : content(_content) {}
    void split();
    void trim();
    void parse();
    void print();
};

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

void Assembler::parse() {
    static regex label("(\\s*(\\w+)\\s*:\\s*).*");
    // 0. remove empty line
    {
        auto i = statements.begin();
        while( i != statements.end() ) {
            if (i->length() < 1) statements.erase(i);
            else i++;
        }
    }
    // 1. extract label:
    size_t i = 0;
    while ( i != statements.size() ) {
        string &str = statements[i];
        smatch result;
        if ( regex_match(str, result, label) ) {
            if ( result.size() == 3 ) {
                labels.insert( { result[2].str(), i } );
            }
            if ( (size_t) result[1].length() == str.length() ) {
                statements.erase(statements.begin()+i);
            } else {
                str.erase( str.find(result[1].str()), result[1].length() );
                i++;
            }
        } else i++;
    }

    for (auto &kv : labels) {
        cout << kv.first << ' ' << kv.second << endl;
    }
}

void Assembler::print() {
    for(auto i = statements.begin(); i != statements.end(); i++) {
        cout << i-statements.begin() << " : " << *i << endl;
    }
}

int main() {
    ifstream fin("test.mips", ios::in);
    string buf, content;
    while(!fin.eof()) {
        getline(fin, buf);
        content += (buf+'\n');
    }
    cout << "Origin:" << endl;
    cout << content;
    cout << "Parsed & trimmed: " << endl;
    Assembler A(content);
    A.split();
    A.trim();
    A.parse();
    A.print();

}
