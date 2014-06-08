#include <iostream>
#include <fstream>
#include <exception>
#include <stdexcept>
#include "simulator.h"

int main(int argc, char **argv) {
    try {
        string filename, mode;
        argv++;
        argc--;
        if ( argc % 2 ) {
            throw invalid_argument("Lack of Arguments!");
        } else {
            map<char, string> arg_map;
            for (int i = 0; i < argc; i+=2) {
                if (argv[i][0] != '-') {
                    throw invalid_argument("Option should start with '-'!");
                }
                arg_map[argv[i][1]] = argv[i+1];
            }
            // -m (run|debug) -f (filename)
            if ( arg_map.count('m') ) {
                mode = arg_map['m'];
            } else {
                throw invalid_argument("Mode should be specified with -m (run|debug).");
            }
            if ( arg_map.count('f') ) {
                filename = arg_map['f'];
            } else {
                throw invalid_argument("File should be specified with -f filename.");
            }
        }

        fstream fin(filename, ios::in);
        fin.exceptions(fin.failbit);

        string ins, content;
        while(!fin.eof()) {
            getline(fin, ins);
            content += ins + "\n";
        }

        Simulator sim(content);
        sim.init();

        if ( mode == "run" ) {
            sim.run();
        } else if ( mode == "debug" ) {
            sim.debug();
        } else {
            throw invalid_argument("Mode should be specified with -m (run|debug).");
        }
    } catch (exception &err) {
        cerr << err.what() << endl;
    }
    return 0;
}
