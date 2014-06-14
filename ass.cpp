#include <iostream>
#include <exception>
#include <stdexcept>
#include "assembler.h"
#include "disassembler.h"

using namespace std;

int main(int argc, char *argv[]) {

    try {
        string filename, type, outputfile;
        ostream *out;
        argv++;
        argc--;
        if ( argc % 2 ) {
            throw invalid_argument("Invalid Arguments!");
        } else {
            map<char, string> arg_map;
            for (int i = 0; i < argc; i+=2) {
                if (argv[i][0] != '-') {
                    throw invalid_argument("Option should start with '-'!");
                }
                arg_map[argv[i][1]] = argv[i+1];
            }
            // -t (coe|plain) -o (outputfilename) -f (filename)
            if ( arg_map.count('t') ) {
                type = arg_map['t'];
            } else {
                type = "plain";
            }
            if ( arg_map.count('f') ) {
                filename = arg_map['f'];
            } else {
                throw invalid_argument("File should be specified with -f filename.");
            }
            if ( arg_map.count('o') ) {
                outputfile = arg_map['o'];
                out = new ofstream(outputfile, ios::out);
            } else {
                out = &cout;
            }
        }

        fstream fin(filename, ios::in);
        fin.exceptions(fin.failbit);

        string ins, content;
        while(!fin.eof()) {
            getline(fin, ins);
            content += ins + "\n";
        }

        Assembler A(content);
        A.split();
        A.trim();
        A.format();
        A.calc_label();
        A.parse();
        A.print(type, *out);

    } catch (exception &err) {
        cerr << err.what() << endl;
    }

    return 0;
}
