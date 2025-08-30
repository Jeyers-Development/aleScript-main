#include <iostream>
#include "interpreter.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: interpreter <program_file>\n";
        return 1;
    }

    Interpreter interpreter;
    if (!interpreter.loadProgram(argv[1])) {
        std::cerr << "Could not load program: " << argv[1] << "\n";
        return 1;
    }

    interpreter.run();
    return 0;
}
