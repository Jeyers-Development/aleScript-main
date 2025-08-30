#include "interpreter.h"
#include <iostream>
#include <string>
#include <sstream>
#include <cmath>


constexpr unsigned int hashStr(const char* str, unsigned int hash = 5381) {
    return *str ? hashStr(str + 1, ((hash << 5) + hash) + *str) : hash;
}
using namespace std;

void Interpreter::executeLine(const std::string& line) {
    auto csline = split(line, SPLAR);
    if (csline.empty()) return;

    switch(hashStr(csline[0].c_str())) {
        case hashStr("print"): {
            executePrint(csline,false);
            break;
        }
        case hashStr("println"): {
            executePrint(csline,true);
            break;
        }

        case hashStr("var"): {
            executeVar(csline);
            break;
        }

        case hashStr("bmath"): {
            executeBMath(csline);
            break;
        }

        case hashStr("input"): {
            executeInput(csline);
            break;
        }

        case hashStr("@REP_IGNORE"): {
            executeRep(false);
            break;
        } case hashStr("@REP"): {
            executeRep(true);
            break;
        }

        default: {
            std::cerr << "Unknown command\n";
            break;
        }
    }

}


void Interpreter::executePrint(const vector<string>& csline) {
    if (csline.size() < 2) break;

    const std::string &arg = csline[1];
    if (!arg.empty() && arg[0] == '"') {
        cout << arg.substr(1);
    } else {
        // treat as variable
        auto it = variables.find(arg);
        if (it != variables.end()) {
            cout << it->second;
        } else {
            cerr << "Variable '" << arg << "' not defined." << endl;
        }
    }
}

void Interpreter::executePrintln(const vector<string>& csline, bool endlB) {
    if (csline.size() < 2) break;

    const std::string &arg = csline[1];
    if (!arg.empty() && arg[0] == '"') {
        cout << arg.substr(1) << endl;
    } else {
        auto it = gloVars.find(arg);
        if (it != gloVars.end()) {
            cout << it->second << endl;
    } else {
            cerr << "Variable '" << arg << "' not defined." << endl;
        }
    }
}

void Interpreter::executeVar(const vector<string>& csline) {
    if (csline.size() == 2) {
        string var = csline[1];
        variables[var] = lastReturned; // assign from last result
    } else if (csline.size() == 3) {
        string var = csline[1];
        variables[var] = csline[2]; // assign direct value
    } else {
        cerr << pc << " ; Invalid var format" << endl;
    }
}

void Interpreter::executeBMath(const vector<string>& csline) {
    if (csline.size() >= 4) {
        float in1 = strtof(variables[csline[1]].c_str(), nullptr);
        string sign = variables[csline[2]];
        float in2 = strtof(variables[csline[3]].c_str(), nullptr);
        float out = 0.0f;

        switch(hashStr(sign.c_str())) {
            case hashStr("+"): out = in1 + in2; break;
            case hashStr("-"): out = in1 - in2; break;
            case hashStr("*"): out = in1 * in2; break;
            case hashStr("/"): out = in1 / in2; break;
            case hashStr("root"): out = pow(in1,1.0/in2); break;
            case hashStr("powr"): out = pow(in1,in2); break;
            default: cerr << "INVALID_OPERATOR\n"; break;
        }

        ostringstream oss;
        oss << out;
        lastReturned = oss.str();
    } else {
        cerr << "Line " << pc << " ; Invalid bmath format." << endl;
        lastReturned = "BMATH_ERROR";
    }
}

void Interpreter::executeInput(const vector<string>& csline) {
    if (csline.size() <= 3) {
        cout << csline[1];
        getline(cin, lastReturned);
        if (lastReturned.empty()) {
            lastReturned = "";
        }
    } else {
        cerr << "Line " << pc << " ; Invalid input format." << endl;
    }
}

void Interpreter::executeRep(bool type) {
    if (type) { // @REP
        pc = pcIgnore;
    } else {    // @REP_IGNORE
        pcIgnore = pc;
    }
}