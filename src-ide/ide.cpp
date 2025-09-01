#include <ncurses.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>

using namespace std;

// Remove surrounding quotes
string trim_quotes(const string &s) {
    if (s.size() >= 2 && ((s.front() == '"' && s.back() == '"') || (s.front() == '\'' && s.back() == '\'')))
        return s.substr(1, s.size() - 2);
    return s;
}

int main() {
    cout << "Enter filepath: ";
    string filename;
    getline(cin, filename);
    filename = trim_quotes(filename);

    ifstream infile(filename);
    if (!infile.is_open()) { cerr << "Failed to open: " << filename << "\n"; return 1; }

    vector<string> lines;
    string line;
    while (getline(infile, line)) lines.push_back(line);
    infile.close();
    if (lines.empty()) lines.emplace_back("");

    int x = 0, y = 0;
    initscr(); raw(); keypad(stdscr, TRUE); noecho(); scrollok(stdscr, TRUE);

    int rows, cols;
    getmaxyx(stdscr, rows, cols);

    while (true) {
        for (int i = 0; i < rows - 2; ++i) {
            move(i, 0); clrtoeol();
            if (i < (int)lines.size()) printw("%s", lines[i].c_str());
        }

        move(rows - 2, 0); clrtoeol();
        printw("^O Save   ^X Exit   Arrows Move   Enter NewLine   Backspace Del   Tab Insert");
        move(y, x);

        int ch = getch();
        if (ch == 24) break;                  // Ctrl+X exit
        else if (ch == 15) {                  // Ctrl+O save
            ofstream outfile(filename);
            for (auto &l : lines) outfile << l << "\n";
            move(rows - 1, 0); clrtoeol(); printw("File saved. Press any key..."); getch();
            break;
        }
        else if (ch == KEY_UP && y > 0) { y--; if (x > (int)lines[y].size()) x = lines[y].size(); }
        else if (ch == KEY_DOWN && y < (int)lines.size() - 1) { y++; if (x > (int)lines[y].size()) x = lines[y].size(); }
        else if (ch == KEY_LEFT) { if (x > 0) x--; else if (y > 0) { y--; x = lines[y].size(); } }
        else if (ch == KEY_RIGHT) { if (x < (int)lines[y].size()) x++; else if (y < (int)lines.size() - 1) { y++; x = 0; } }
        else if (ch == KEY_BACKSPACE || ch == 127) {
            if (x > 0) { lines[y].erase(x - 1, 1); x--; }
            else if (y > 0) { x = lines[y - 1].size(); lines[y - 1] += lines[y]; lines.erase(lines.begin() + y); y--; }
        }
        else if (ch == '\n') { lines.insert(lines.begin() + y + 1, lines[y].substr(x)); lines[y] = lines[y].substr(0, x); y++; x = 0; }
        else if (ch == '\t') {
            const int tabSize = 4;
            lines[y].insert(x, tabSize, ' ');
            x += tabSize;
        }
        else if (isprint(ch)) { lines[y].insert(x, 1, ch); x++; }
    }

    endwin();
    return 0;
}
