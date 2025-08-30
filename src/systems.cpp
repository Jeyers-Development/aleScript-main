#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include "main.h"
using namespace std;

string replaceFirstOccurrence(string str, char oldChar, char newChar) {
    size_t pos = str.find(oldChar);
    if (pos != string::npos) {
        str[pos] = newChar;
    }
    return str;
}

string removeFirstOccurrence(string str, char target) {
    size_t pos = str.find(target);
    if (pos != string::npos) {
        str.erase(pos, 1);
    }
    return str;
}

vector<string> split(const string& input, char delimiter) {
    vector<string> parts;
    stringstream ss(input);
    string item;
    while (getline(ss, item, delimiter)) {
        if (!item.empty()) {
            parts.push_back(item);
        }
    }
    return parts;
}

string aleMathCalculation(float in1, string sign, float in2) {
    float out = 0.0f;
    switch (hashStr(sign.c_str())) {
        case hashStr("+"):    out = in1 + in2; break;
        case hashStr("-"):    out = in1 - in2; break;
        case hashStr("*"):    out = in1 * in2; break;
        case hashStr("/"):    out = in1 / in2; break;
        case hashStr("sqrt"): out = sqrt(in1); break;
        case hashStr("powr"): out = pow(in1, in2); break;
        default: return "";
    }

    ostringstream oss;
    oss << out;
    return oss.str();
}

bool aleIfStatement(string in1, string oper, string in2) {
    bool out = false;
    float fIn1 = stof(in1);
    float fIn2 = stof(in2);

    switch (hashStr(oper.c_str())) {
        case hashStr("=="): if(in1==in2)   out=true; break;
        case hashStr(">"):  if(fIn1>fIn2)  out=true; break;
        case hashStr("<"):  if(fIn1<fIn2)  out=true; break;
        case hashStr(">="): if(fIn1>=fIn2) out=true; break;
        case hashStr("<="): if(fIn1<=fIn2) out=true; break;
        default: return false;
    }

    return out;
}