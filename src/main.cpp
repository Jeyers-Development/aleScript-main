#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <string>
#include <unordered_map>
#include <vector>
#include "main.h"

using namespace std;

string AleScriptVersion = "v1.1";

unordered_map<string, string> gloVars;
vector<string> program_lines;
string lastReturned = "NULL";

char SPLAR = '?';
int pc = 0;
int pcIgnore = 0;
bool pyUtilized = false;

int main(int argc, char* argv[]) {
    gloVars["EMPTY"] = "";
    gloVars["ALE_VERSION"] = AleScriptVersion;

    if (argc < 2) {
        cerr << "Usage: interpreter <program_file>\n";
        return 1;
    }

    ifstream programFile(argv[1]);
    if (!programFile) {
        cerr << "Could not open file: " << argv[1] << "\n";
        return 1;
    }

    string line;
    while (getline(programFile, line)) {
        // Strip trailing whitespace
        while (!line.empty() && isspace(line.back())) line.pop_back();
        program_lines.push_back(line);
    }

    while (pc < (int)program_lines.size()) {
        string cline = program_lines[pc];
        pc++;

        // Skip empty lines
        if (cline.empty() || cline[0]=='#') continue;

        auto csline = split(cline, SPLAR);

        switch (hashStr(csline[0].c_str())) {
            case hashStr("@SET_SPLAR"): {
                if (csline.size() >= 2 && !csline[1].empty()) {
                    SPLAR = csline[1][0];
                } else {
                    cerr << pc << " ; Invalid @SET_SPLAR usage\n";
                }
                break;
            }

            case hashStr("@REP_IGNORE"): {
                pcIgnore = pc;
                break;
            }

            case hashStr("@REP"): {
                pc = pcIgnore;
                break;
            }

            case hashStr("print"): {
                if (csline.size() < 2) break;

                const std::string &arg = csline[1];
                if (!arg.empty() && arg[0] == '"') {
                    cout << arg.substr(1);
                } else {
                    // treat as variable
                    auto it = gloVars.find(arg);
                    if (it != gloVars.end()) {
                        cout << it->second;
                    } else {
                        cerr << "Variable '" << arg << "' not defined." << endl;
                    }
                }
                break;
            }

            case hashStr("println"): {
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
                break;
            }


            case hashStr("var"): {
                if (csline.size() == 2) {
                    string var = csline[1];
                    gloVars[var] = lastReturned; // assign from last result
                } else if (csline.size() == 3) {
                    string var = csline[1];
                    gloVars[var] = csline[2]; // assign direct value
                } else {
                    cerr << pc << " ; Invalid var format: " << cline << endl;
                }
                break;
            }

            case hashStr("math"): {
                if (csline.size() >= 4) { // expect: math ?var1 ?sign ?var2
                    float in1 = 0, in2 = 0;
                    std::string sign;

                    // Operand 1
                    auto it1 = gloVars.find(csline[1]);
                    if (it1 != gloVars.end()) {
                        in1 = strtof(it1->second.c_str(), nullptr);
                    } else {
                        cerr << pc << " ; Variable '" << csline[1] << "' not found.\n";
                        break;
                    }

                    // Operator
                    auto itSign = gloVars.find(csline[2]);
                    if (itSign != gloVars.end()) {
                        sign = itSign->second;
                    } else {
                        sign = csline[2]; // allow direct operator like "+"
                    }

                    // Operand 2
                    auto it2 = gloVars.find(csline[3]);
                    if (it2 != gloVars.end()) {
                        in2 = strtof(it2->second.c_str(), nullptr);
                    } else {
                        cerr << pc << " ; Variable '" << csline[3] << "' not found.\n";
                        break;
                    }

                    lastReturned = aleMathCalculation(in1, sign, in2);
                    if (lastReturned.empty()) lastReturned = "ALEMATHCALCULATION_ERROR";

                } else {
                    cerr << pc << " ; Invalid math format: " << cline << endl;
                }
                break;
            }

                        
            case hashStr("input"): {
                if (csline.size() <= 3) {
                    cout << csline[1];
                    getline(cin, lastReturned);
                    if (lastReturned.empty()) {
                        lastReturned = "";
                    }
                }
                break;
            }

            case hashStr("if"): {
                if (csline.size() >= 5) {
                    string in1 = gloVars[csline[1]];
                    string in2 = gloVars[csline[3]];
                    
                    if (aleIfStatement(in1, csline[2], in2)) 
                        lastReturned = "true";
                    else {
                        lastReturned = "false";
                        pc = pc + stoi(csline[4]);
                    }
                } else {
                    cerr << pc << " ; Invalid if statement: " << cline << endl;
                }
                break;
            }

            case hashStr("system"): {
                if (csline.size() >= 2) {
                    system(csline[1].c_str());
                } else {
                    cerr << pc << " ; Invalid system format: " << cline << endl;
                }
                break;
            }

            case hashStr("ceil"): {
                if (csline.size() >= 2) {
                    ceil(stof(csline[1]));
                } else {
                    cerr << pc << " ; Invalid ceil format: " << cline << endl;
                }
                break;
            }

            case hashStr("floor"): {
                if (csline.size() >= 2) {
                    floor(stof(csline[1]));
                } else {
                    cerr << pc << " ; Invalid ceil format: " << cline << endl;
                }
                break;
            }

            default: {
                cout << "Line>" << pc << " ; Unknown command\n";
                break;
            }
        }

    }


    return 0;
}
