#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional> 
#include <iostream>

using namespace std;

class Interpreter {
public:
    Interpreter(const string& programPath);

    bool loadProgram(const string& filename);
    void run();

    void executeLine(const string& line);
    void executePrint(const vector<string>& csline);
    void executePrintln(const vector<string>& csline);
    void executeVar(const vector<string>& csline);
    void executeBMath(const vector<string>& csline);
    void executeInput(const vector<string>& csline);
    void executeRep(bool type);
    void executeFWrite(const vector<string>& csline);
    void executeJoin(const vector<string>& csline);
    void executeSET_SPLAR(const vector<string>& csline);
    void executeSystem(const vector<string>& csline);
    void executeIfStatement(const vector<string>& csline);

    string getValue(const string& s) const {
        if (!s.empty() && s.front() == '"') {
            return s.substr(1);
        } else {
            auto it = variables.find(s);
            if (it != variables.end()) {
                return it->second;
            } else {
                cerr << "Variable '" << s << "' is not declared." << endl;
                return "GETVALUE_ERROR";
            }
        }
    }


private:

    unordered_map<string, string> variables;
    vector<string> programLines;
    string lastReturned;
    char SPLAR;
    int pc;
    int pcIgnore;

    vector<string> split(const string& line, char delimiter);
    void trim(string& str);

    unordered_map<string, function<void(const vector<string>&)>> extraBrews;
};