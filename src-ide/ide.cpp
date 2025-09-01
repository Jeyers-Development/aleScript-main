#include <ncurses.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#if defined(_WIN32)
#include <windows.h>
#else
#include <cstdlib>
#endif

using namespace std;

const char* ide_version = "v1.1.0";

// Remove surrounding quotes
string trim_quotes(const string &s) {
    if (s.size() >= 2 && ((s.front() == '"' && s.back() == '"') || (s.front() == '\'' && s.back() == '\'')))
        return s.substr(1, s.size() - 2);
    return s;
}

// List of words for autocomplete
vector<string> autocomplete_words = {
    "EMPTY",
    "ALE_VERSION",
    "ARG_PATH",
    "HELP_BMATH_OPERATORS",
    "HELP_IF_OPRS",
    "LAST_RETURNED",

    "println",
    "bmath",
    "input",
    "@REP_IGNORE",
    "@REP",
    "@SET_SPLAR",
    "fwrite",
    "join",
    "system",
    "round",
    "webget",
    "getFps",
    "exit",
};

// Get last n characters of string
string last_n(const string &s, int n) {
    if ((int)s.size() < n) return s;
    return s.substr(s.size() - n);
}

int main(int argc, char** argv) {
    string filename;

    if (argc >= 2) {
        filename = argv[1];
    } else {
        #if defined(__unix__)
            cout << "Ale IDE " << ide_version << " - Maximize Window if Not Already!" << endl;
        #else
            cout << "Ale IDE " << ide_version << endl;
        #endif
        cout << "Enter filepath: ";
        getline(cin, filename);
        filename = trim_quotes(filename);
    }

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

    int menu_rows = 2;
    int visible_rows = rows - menu_rows;
    int scroll = 0;

    string search_query;
    string replace_query;
    int search_line = -1;

    while (true) {
        clear();

        // Draw menu
        mvprintw(0, 0, "^O Save  ^X Exit   ^R Refresh   ^F Search   ^H Replace   n Next   r ReplaceOne   a ReplaceAll   Arrows Move");
        mvprintw(1, 0, "<=-~-=-~-=-~-=-~-=-~-=-~-=-~-=-~-=-~-=-~-=-~-=-~-=-~-=-~-=-~-=-~-=-~-=-~-=-~-=-~-=-~-=-~-=-~-=-~-=-~-=-~-=>");

        // Adjust scroll
        if (y < scroll) scroll = y;
        if (y >= scroll + visible_rows) scroll = y - visible_rows + 1;

        // Draw visible file lines
        for (int i = 0; i < visible_rows; ++i) {
            int idx = i + scroll;
            move(i + menu_rows, 0); clrtoeol();
            if (idx < (int)lines.size()) {
                if (!search_query.empty() && idx == search_line) {
                    // Highlight search match
                    string &l = lines[idx];
                    int pos = l.find(search_query);
                    if (pos != string::npos) {
                        printw("%.*s", pos, l.c_str());
                        attron(A_REVERSE);
                        printw("%.*s", (int)search_query.size(), l.c_str() + pos);
                        attroff(A_REVERSE);
                        printw("%s", l.c_str() + pos + search_query.size());
                    } else {
                        printw("%s", l.c_str());
                    }
                } else {
                    printw("%s", lines[idx].c_str());
                }
            }
        }

        // Move cursor relative to scroll
        move(y - scroll + menu_rows, x);

        int ch = getch();
        if (ch == 24) break;                // Ctrl+X
        else if (ch == 15) {                // Ctrl+O save then exit
            ofstream outfile(filename);
            for (auto &l : lines) outfile << l << "\n";
            move(rows - 1, 0); clrtoeol();
            printw("File saved. Press any key...");
            getch();
            break;
        }
        else if (ch == 18) {                // Ctrl+R to refresh
            #ifdef _WIN32
                system("cls");
            #elif defined(__unix__)
                system("clear");
            #endif
            getmaxyx(stdscr, rows, cols);
            resizeterm(rows, cols);
            clear();
        }
        else if (ch == 6) { // Ctrl+F search
            echo();
            curs_set(1);
            move(rows - 1, 0); clrtoeol();
            printw("Search: ");
            char buffer[256];
            getnstr(buffer, 255);
            noecho();
            search_query = buffer;
            search_line = -1;

            // Find first match
            for (int i = 0; i < (int)lines.size(); i++) {
                size_t pos = lines[i].find(search_query);
                if (pos != string::npos) {
                    search_line = i;
                    y = i;
                    x = pos;
                    break;
                }
            }
        }
        else if (ch == 8) { // Ctrl+H replace
            echo();
            curs_set(1);
            move(rows - 1, 0); clrtoeol();
            printw("Find: ");
            char buffer1[256];
            getnstr(buffer1, 255);
            search_query = buffer1;

            move(rows - 1, 0); clrtoeol();
            printw("Replace with: ");
            char buffer2[256];
            getnstr(buffer2, 255);
            replace_query = buffer2;
            noecho();
            search_line = -1;

            // Jump to first match
            for (int i = 0; i < (int)lines.size(); i++) {
                size_t pos = lines[i].find(search_query);
                if (pos != string::npos) {
                    search_line = i;
                    y = i;
                    x = pos;
                    break;
                }
            }
        }
        else if (ch == 'n' && !search_query.empty()) { // next result
            int start = (search_line == -1) ? y : search_line + 1;
            search_line = -1;
            for (int i = start; i < (int)lines.size(); i++) {
                size_t pos = lines[i].find(search_query);
                if (pos != string::npos) {
                    search_line = i;
                    y = i;
                    x = pos;
                    break;
                }
            }
        }
        else if (ch == 'r' && !search_query.empty() && search_line != -1) { // replace one
            size_t pos = lines[search_line].find(search_query, x);
            if (pos != string::npos) {
                lines[search_line].replace(pos, search_query.size(), replace_query);
                x = pos + replace_query.size();
            }
        }
        else if (ch == 'a' && !search_query.empty()) { // replace all
            for (auto &l : lines) {
                size_t pos = 0;
                while ((pos = l.find(search_query, pos)) != string::npos) {
                    l.replace(pos, search_query.size(), replace_query);
                    pos += replace_query.size();
                }
            }
            search_line = -1;
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
            string last3 = last_n(lines[y].substr(0, x), 3);
            bool did_autocomplete = false;
            if (last3.size() == 3) {
                for (auto &word : autocomplete_words) {
                    if (word.substr(0, 3) == last3 && word.size() > 3) {
                        string to_insert = word.substr(3);
                        lines[y].insert(x, to_insert);
                        x += to_insert.size();
                        did_autocomplete = true;
                        break;
                    }
                }
            }
            if (!did_autocomplete) {
                const int tabSize = 4;
                lines[y].insert(x, tabSize, ' ');
                x += tabSize;
            }
        }
        else if (isprint(ch)) { lines[y].insert(x, 1, ch); x++; }
    }

    endwin();
    return 0;
}