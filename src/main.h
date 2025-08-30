#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <cmath>
using namespace std;

string replaceFirstOccurrence(string str, char oldChar, char newChar);
string removeFirstOccurrence(string str, char target);
vector<string> split(const string& input, char delimiter);
string aleMathCalculation(float in1, string sign, float in2);
bool aleIfStatement(string in1, string oper, string in2);

extern unordered_map<string, string> gloVars;
extern vector<string> program_lines;
extern string lastReturned;
extern string cline;
extern char SPLAR;
extern int pc;
extern int pcIgnore;
extern bool pyUtilized;

constexpr unsigned int hashStr(const char* str, unsigned int h = 5381) {
    return (*str) ? hashStr(str + 1, ((h << 5) + h) + *str) : h;
}