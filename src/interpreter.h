// interpreter.h
#pragma once
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;


class Interpreter {
public:
    Interpreter();

    bool loadProgram(const string& filename);
    void run();

    // Command handler declarations (needed for brews.cpp)
    void executeLine(const string& line);
    void executePrint(const vector<string>& csline);
    void executePrintln(const vector<string>& csline);
    void executeVar(const vector<string>& csline);
    void executeBMath(const vector<string>& csline);
    void executeInput(const vector<string>& csline);
    void executeRep(bool type);

private:
    unordered_map<string, string> variables;
    vector<string> programLines;
    string lastReturned;
    char SPLAR;
    int pc;
    int pcIgnore;

    vector<string> split(const string& line, char delimiter);
    float getValue(const string& token);
    void trim(string& str);
};
