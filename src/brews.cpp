#include "interpreter.h"
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <chrono>
#include <string>
#include <sstream>
#include <cmath>

constexpr unsigned int hashStr(const char* str, unsigned int hash = 5381) {
    return *str ? hashStr(str + 1, ((hash << 5) + hash) + *str) : hash;
}
using namespace std;

void Interpreter::executeLine(const string& line) {
    variables["LAST_RETURNED"] = lastReturned;

    auto csline = split(line, SPLAR);
    if (csline.empty() || csline[0].rfind("//", 0) == 0) return;

    switch(hashStr(csline[0].c_str())) {
        case hashStr("print"): {
            executePrint(csline);
            break;
        }
        case hashStr("println"): {
            executePrintln(csline);
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
        } case hashStr("@SET_SPLAR"): {
            executeSET_SPLAR(csline);
            break;
        }
        
        case hashStr("fwrite"): {
            executeFWrite(csline);
            break;
        }

        case hashStr("join"): {
            executeJoin(csline);
            break;
        }

        case hashStr("system"): {
            executeSystem(csline);
            break;
        }

        case hashStr("if"): {
            executeIfStatement(csline);
            break;
        }

        case hashStr("round"): {
            executeRounding(csline);
            break;
        }

        case hashStr("webget"): {
            executeWebGet(csline);
            break;
        }

        case hashStr("getFps"): {
            executeGetFPS();
            break;
        }

        case hashStr("exit"): { exit(0); break; }

        default: {
            auto it = extraBrews.find(csline[0]);
            if (it != extraBrews.end()) {
                it->second(csline); // run the extra brew
            } else {
                cerr << "Line " << pc << " ; Unknown grain > " << csline[0] << endl;
            }
            break;
        }
    }

}

// println?<"value/var>
void Interpreter::executePrint(const vector<string>& csline) {
    if (csline.size() < 2) {
        cerr << "Line " << pc << " ; Invalid 'print' format" << endl;
        exit(0);
    }
    cout << getValue(csline[1]);
}

// println?<"value/var>
void Interpreter::executePrintln(const vector<string>& csline) {
    if (csline.size() < 2) {
        cerr << "Line " << pc << " ; Invalid 'println' format" << endl;
        exit(0);
    }
    cout << getValue(csline[1]) << endl;
}

// var?<var name>?<var value>   ---   ?<var value> <-- Optional
void Interpreter::executeVar(const vector<string>& csline) {
    if (csline.size() < 2) { cerr << "Line " << pc << " ; Invalid \'var\' format" << endl; exit(0); }

    variables[csline[1]] = (csline.size() == 2) ? lastReturned : getValue(csline[2]);
}

// bmath?<num1>?<operator>?<num2>
void Interpreter::executeBMath(const vector<string>& csline) {
    if (csline.size() >= 4) {
        float in1 = strtof(getValue(csline[1]).c_str(), nullptr);
        string sign = getValue(csline[2]);
        float in2 = strtof(getValue(csline[3]).c_str(), nullptr);
        string sout = "INVALID_OPERATOR";
        float out = numeric_limits<float>::quiet_NaN();

        switch(hashStr(sign.c_str())) {
            case hashStr("+"): out = in1 + in2; break;
            case hashStr("-"): out = in1 - in2; break;
            case hashStr("*"): out = in1 * in2; break;
            case hashStr("/"): out = in1 / in2; break;
            case hashStr("root"): out = pow(in1, 1.0 / in2); break;
            case hashStr("powr"): out = pow(in1, in2); break;
            default: break;
        }

        ostringstream oss;
        if (!isnan(out)) oss << out;
        else oss << sout;
        lastReturned = oss.str();
    } else {
        cerr << "Line " << pc << " ; Invalid 'bmath' format." << endl;
        lastReturned = "BMATH_ERROR";
    }
}

// input?<texttoput>
void Interpreter::executeInput(const vector<string>& csline) {
    if (csline.size() < 2) { cerr << "Line " << pc << " ; Invalid \'input\' format." << endl; exit(0); }
    cout << getValue(csline[1]);
    getline(cin, lastReturned);
    if (lastReturned.empty()) {
        lastReturned = "";
    }
}

// @REP ; @REP_IGNORE
void Interpreter::executeRep(bool type) {
    if (type) { // @REP
        pc = pcIgnore;
    } else {    // @REP_IGNORE
        pcIgnore = pc;
    }
}


// fwrite?<filepath>?<mode>?<contents>
void Interpreter::executeFWrite(const vector<string>& csline) {
    if (csline.size() < 3) { cerr << "Line " << pc << " ; Invalid \'fwrite\' format." << endl; exit(0); }

    string filepath = getValue(csline[1]);
    string mode = getValue(csline[2]);
    string fileContents = csline.size() > 3 ? getValue(csline[3]) : "";


    if (mode == "w") {
        ofstream(filepath, ios::out | ios::trunc) << fileContents << endl;
        lastReturned = "FWRITE_WRITE_TO_FILE";
    } 
    else if (mode == "a") {
        ofstream(filepath, ios::out | ios::app) << fileContents << endl;
        lastReturned = "FWRITE_APPEND_TO_FILE";
    } 
    else if (mode == "r") {
        ifstream inFile(filepath);
        if (inFile) lastReturned = string((istreambuf_iterator<char>(inFile)), istreambuf_iterator<char>());
    }
}

// join?<var1>?<var2>? --> <more vars>?
void Interpreter::executeJoin(const vector<string>& csline) {
    if (csline.size() < 2) {
        cerr << "Line " << pc << " ; Invalid 'join' format." << endl;
        return;
    }

    lastReturned.clear();
    for (size_t i = 1; i < csline.size(); ++i)
        lastReturned += !csline[i].empty() && csline[i][0] == '"' ? csline[i].substr(1) : variables[csline[i]];
}

// @SET_SPLAR?<new SPLAR>
void Interpreter::executeSET_SPLAR(const vector<string>& csline) {
    if (csline.size() < 2 || csline[1].empty()) cerr << "@SET_SPLAR requires a character argument" << endl;
    SPLAR = this->getValue(csline[1])[0];
}

// system?<command>
void Interpreter::executeSystem(const vector<string>& csline) {
    if (csline.size() < 2 || csline[1].empty()) cerr << "Line " << pc << " ; Invalid \'system\' format." << endl;
    system(getValue(csline[1]).c_str());
}

// systemsil?<command>
void Interpreter::executeSystemsil(const vector<string>& csline) {
    std::string cmd = getValue(csline[1]);
    #ifdef _WIN32
        cmd += " > NUL 2>&1";
    #else
        cmd += " > /dev/null 2>&1";
    #endif
    system(cmd.c_str());
}


// if?<in1>?<opr>?<in2>?<lines to move if false>
void Interpreter::executeIfStatement(const vector<string>& csline) {
    if (csline.size() < 2 || csline[1].empty()) cerr << "Line " << pc << " ; Invalid \'system\' format." << endl;

    string in1 = getValue(csline[1]);
    string in2 = getValue(csline[3]);
    string opr = getValue(csline[2]);
    int linesTM = stoi(getValue(csline[4]));
    bool out = false;

    switch (hashStr(opr.c_str())) {
        case hashStr("=="): if (in1==in2)out=true; break;
        case hashStr("!="): if (in1!=in2)out=true; break;
        case hashStr(">"): if (stof(in1)>stof(in2))out=true; break;
        case hashStr("<"): if (stof(in1)<stof(in2))out=true; break;
        case hashStr(">="): if (stof(in1)>=stof(in2))out=true; break;
        case hashStr("<="): if (stof(in1)<=stof(in2))out=true; break;
        default: cerr << "Line " << pc << " ; Invalid \'if\' operator." << endl; break;
    }
    if (!out) {
        pc = pc + linesTM;
    }

    lastReturned = out ? "true" : "false";
}

// round?<num>?<floor/ceil>
void Interpreter::executeRounding(const vector<string>& csline) {
    if (csline.size() < 3 || csline[1].empty()) cerr << "Line " << pc << " ; Invalid \'round\' format." << endl;

    float num = stof(getValue(csline[1]));
    string type = getValue(csline[2]);

    if (type == "floor") lastReturned = to_string((int)floor(num));
    else if (type == "ceil") lastReturned = to_string((int)ceil(num));
    else lastReturned = "INVALID_ROUND_TYPE";
}

// webget?<url>?<path to save>
void Interpreter::executeWebGet(const vector<string>& csline) {
    std::string url = getValue(csline[1]);
    std::string output = getValue(csline[2]);

    #ifdef _WIN32
        std::string command = "curl -s -o \"" + output + "\" \"" + url + "\"";
    #else
        std::string command = "wget -q -O \"" + output + "\" \"" + url + "\"";
    #endif

    system(command.c_str());
}


void Interpreter::executeGetFPS() {
    using namespace std::chrono;

    fpsFrames++;
    auto now = high_resolution_clock::now();
    duration<float> elapsed = now - fpsLastTime;

    if (elapsed.count() >= 0.01f) {
        lastFPS = fpsFrames / elapsed.count();
        fpsFrames = 0;
        fpsLastTime = now;
    }

    lastReturned = std::to_string(lastFPS);
}