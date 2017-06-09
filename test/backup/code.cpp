/*
 * linker.cpp
 *
 *  Created on: Feb 7, 2016
 *      Author: raghu
 */
#include <iostream>
#include <cstring>
#include <sstream>
#include <cstdio>
#include <string>
#include <vector>
#include <cctype>
#include <queue>
#include <map>
using namespace std;

const string ERROR_SYMBOL = "__ERROR__";
const int MAX_INSTRUCTION = 512;

typedef struct INSTRUCTION {
    string c;
    string instr;
} Instruction;

typedef struct SYMBOL {
    string name;
    int val;
} Symbol;

typedef struct TOKEN {
    string s;
    int line;
    int offset;
} Token;

vector<int> moduleAddr;
vector<vector<Instruction> > instr;
vector<vector<Symbol> > defineSym;
vector<vector<string> > useSym;

map<string, int> symbolTable;
map<string, int> usedDefineSym;

queue<Token> tokens;

void printParsedResults() {
    cout << "Bases of Modules:" << endl;
    for (int i = 0; i < moduleAddr.size(); ++i) {
        cout << moduleAddr[i] << endl;
    }

    cout << "-----------------" << endl;
    cout << "Define List:" << endl;
    for (int i = 0; i < defineSym.size(); ++i) {
        cout << "Module " << i + 1 << ":" << endl;
        for (int j = 0; j < defineSym[i].size(); ++j) {
            cout << defineSym[i][j].name << " " << defineSym[i][j].val << endl;
        }
    }

    cout << "-----------------" << endl;
    cout << "Use List:" << endl;
    for (int i = 0; i < useSym.size(); ++i) {
        cout << "Module " << i + 1 << ":" << endl;
        for (int j = 0; j < useSym[i].size(); ++j) {
            cout << useSym[i][j] << endl;
        }
    }

    cout << "-----------------" << endl;
    cout << "Instructions:" << endl;
    for (int i = 0; i < instr.size(); ++i) {
        cout << "Module " << i + 1 << ":" << endl;
        for (int j = 0; j < instr[i].size(); ++j) {
            cout << instr[i][j].c << " " << instr[i][j].instr << endl;
        }
    }
}

void tokenize() {
    string in;
    int lineCnt = 0;
    int lastOffset = 0;
    int lastLen = 1;
    bool empty;
    while (getline(cin, in)) {
        empty = true;
        ++lineCnt;
        /*
        if (strcmp(in.c_str(), "") == 0) {
            lastOffset = 1;
            continue;
        }
        */
        int len = in.size();
        for (int i = 0; i < len; ++i) {
            if (!isspace(in[i])) {
                empty = false;
                int j;
                for (j = i; j < len; ++j) {
                    if (isspace(in[j])) {
                        break;
                    }
                }
                Token t;
                t.s = in.substr(i, j - i);
                t.line = lineCnt;
                t.offset = i + 1;
                lastOffset = t.offset + t.s.length();
                tokens.push(t);
                i = j;
            }
        }
        lastLen = len;
    }
    Token t;
    t.s = ERROR_SYMBOL;
    t.line = lineCnt;
    t.offset = empty ? lastLen > 0 ? lastLen : 1 : lastOffset;
    tokens.push(t);
}

void parseerror(int linenum, int lineoffset, int errcode) {
    static string errstr[] = {
        "NUM_EXPECTED",
        "SYM_EXPECTED",
        "ADDR_EXPECTED",
        "SYM_TOLONG",
        "TO_MANY_DEF_IN_MODULE",
        "TO_MANY_USE_IN_MODULE",
        "TO_MANY_INSTR"
    };
    printf("Parse Error line %d offset %d: %s\n", linenum, lineoffset, errstr[errcode].c_str());
}

int checkNum(string in) {
    int n = 0;
    int l = in.size();

    for (int i = 0; i < l; ++i) {
        if (!isdigit(in[i])) {
            n = -1;
            break;
        } else {
            n = n * 10 + in[i] - '0';
        }
    }
    return n;
}

string checkSym(string in) {
    int len = in.length();

    if (!isalpha(in[0])) {
        return ERROR_SYMBOL;
    }
    for (int i = 1; i < len; ++i) {
        if (!isalnum(in[i])) {
            return ERROR_SYMBOL;
        }
    }
    return in;
}

string checkAddressing(string in) {
    if (in.length() > 1) {
        return ERROR_SYMBOL;
    } else {
        if (in[0] != 'I' && in[0] != 'A' &&
                in[0] != 'R' && in[0] != 'E') {
            return ERROR_SYMBOL;
        } else {
            return in;
        }
    }
}

bool checkOpCode(string addr) {
    int  len = addr.length();
    bool ret = true;
    for (int i = 0; i < len; ++i) {
        if (!(addr[i] >= '0' && addr[i] <= '9')) {
            ret = false;
            break;
        }
    }
    return ret && (len <= 4);
}

bool parse() {
    int len = tokens.size();
    int instrCnt = 0;

    while (tokens.size() > 1) {
        moduleAddr.push_back(instrCnt);

        vector<Symbol> defineSymbols;
        vector<string> useSymbols;
        vector<Instruction> instructions;

        // parse defcount
        Token t = tokens.front();
        tokens.pop();
        if (tokens.size() == 0) {
            parseerror(t.line, t.offset, 0);
            return false;
        }

        int defcount = checkNum(t.s);
        if (defcount == -1) {
            parseerror(t.line, t.offset, 0);
            return false;
        } else if (defcount > 16) {
            parseerror(t.line, t.offset, 4);
            return false;
        }
        // cout << "defcount: " << defcount << endl;

        for (int i = 0; i < defcount; ++i) {
            // check symbol
            t = tokens.front();
            tokens.pop();
            if (tokens.size() == 0) {
                parseerror(t.line, t.offset, 1);
                return false;
            }

            string sym = checkSym(t.s);
            if (sym == ERROR_SYMBOL) {
                parseerror(t.line, t.offset, 1);
                return false;
            } else if (sym.length() > 16) {
                parseerror(t.line, t.offset, 3);
                return false;
            }

            // check value for symbol
            t = tokens.front();
            tokens.pop();
            if (tokens.size() == 0) {
                parseerror(t.line, t.offset, 0);
                return false;
            }

            int val = checkNum(t.s);
            if (val == -1) {
                parseerror(t.line, t.offset, 0);
                return false;
            }

            Symbol symbol;
            symbol.name = sym;
            symbol.val = val;

            defineSymbols.push_back(symbol);
        }
        defineSym.push_back(defineSymbols);


        // parse usecount
        t = tokens.front();
        tokens.pop();
        if (tokens.size() == 0) {
            parseerror(t.line, t.offset, 0);
            return false;
        }

        int usecount = checkNum(t.s);
        if (usecount == -1) {
            parseerror(t.line, t.offset, 0);
            return false;
        } else if (usecount > 16) {
            parseerror(t.line, t.offset, 5);
            return false;
        }

        // cout << "usecount: " << usecount << endl;
        for (int i = 0; i < usecount; ++i) {
            // check symbol
            t = tokens.front();
            tokens.pop();

            if (tokens.size() == 0) {
                parseerror(t.line, t.offset, 1);
                return false;
            }

            // cout << t.s << " " << t.s.length() << endl;

            string sym = checkSym(t.s);
            if (sym == ERROR_SYMBOL) {
                parseerror(t.line, t.offset, 1);
                return false;
            } else if (sym.length() > 16) {
                parseerror(t.line, t.offset, 3);
                return false;
            }
            useSymbols.push_back(sym);
        }
        useSym.push_back(useSymbols);

        // parse codecount
        t = tokens.front();
        tokens.pop();

        if (tokens.size() == 0) {
            parseerror(t.line, t.offset, 0);
            return false;
        }

        int codecount = checkNum(t.s);
        if (codecount == -1) {
            parseerror(t.line, t.offset, 0);
            return false;
        } else if (codecount + instrCnt > MAX_INSTRUCTION) {
            parseerror(t.line, t.offset, 6);
            return false;
        }
        // cout << "codecount: " << codecount << endl;

        for (int i = 0; i < codecount; ++i) {
            ++instrCnt;

            // check addressing
            t = tokens.front();
            tokens.pop();

            if (tokens.size() == 0) {
                parseerror(t.line, t.offset, 2);
                return false;
            }

            string ad = checkAddressing(t.s);
            if (ad == ERROR_SYMBOL) {
                parseerror(t.line, t.offset, 2);
            }

            // check addr
            t = tokens.front();
            tokens.pop();

            if (tokens.size() == 0 || checkNum(t.s) == -1) {
                parseerror(t.line, t.offset, 0);
                return false;
            }

            string addr = t.s;

            Instruction ins;
            ins.c = ad;
            ins.instr = addr;
            instructions.push_back(ins);
        }
        instr.push_back(instructions);
    }
    moduleAddr.push_back(instrCnt);
    return true;
}

void printDefineSymbols() {
    map<string, int> symbolHash;
    vector<string> output;

    string errorMsg = " Error: This variable is multiple times defined; first value used";

    for (int i = 0; i < defineSym.size(); ++i) {
        for (int j = 0; j < defineSym[i].size(); ++j) {
            Symbol sym = defineSym[i][j];
            int hCode = symbolHash[sym.name];
            if (hCode == 0) {
                symbolHash[sym.name] = output.size() + 1;
                stringstream addr;
                int offset = sym.val + moduleAddr[i];
                if (offset >= moduleAddr[i + 1]) {
                    cout << "Warning: Module " << i + 1 << ": " <<
                        sym.name << " to big " << offset << " (max=" <<
                        moduleAddr[i + 1] - 1 << ") assume zero relative" << endl;
                        symbolTable[sym.name] = 1;
                        output.push_back(sym.name + "=" + "0");
                } else {
                    addr << offset;
                    symbolTable[sym.name] = sym.val + moduleAddr[i] + 1;
                    output.push_back(sym.name + "=" + addr.str());
                }
            } else {
                if (hCode != -1) {
                    output[hCode - 1] += errorMsg;
                    symbolHash[sym.name] = -1;
                }
                defineSym[i].erase(defineSym[i].begin() + j);
                --j;
            }
        }
    }

    cout << "Symbol Table" << endl;
    for (int i = 0; i < output.size(); ++i) {
        cout << output[i] << endl;
    }
}

string redirect(Instruction in, int mi, map<string, int>& usedUseSym) {
    string c = in.c;
    string addr = in.instr;

    if (c == "I") {
        bool e = checkOpCode(addr);
        if (!e) {
            addr = "9999 Error: Illegal immediate value; treated as 9999";
        } else {
            char dest[4];
            stringstream ss;
            int n = 0;
            for (int i = 0; i < addr.size(); ++i) {
                n = n * 10 + addr[i] - '0';
            }
            sprintf(dest, "%04d", n);
            ss << dest;
            addr = ss.str();
        }
    } else if (c == "A") {
        bool e = checkOpCode(addr);
        if (!e) {
            addr = "9999 Error: Illegal opcode; treated as 9999";
        } else {
            while (addr.size() < 4) {
                addr = "0" + addr;
            }
            int n = 0;
            for (int i = 1; i < addr.size(); ++i) {
                n = n * 10 + addr[i] - '0';
            }
            if (n >= MAX_INSTRUCTION) {
                addr = string(1, addr[0]) +
                    "000 Error: Absolute address exceeds machine size; zero used";
            } else {
                char dest[4];
                stringstream ss;
                sprintf(dest, "%03d", n);
                ss << dest;
                addr = string(1, addr[0]) + ss.str();
            }
        }
    } else if (c == "R") {
        bool e = checkOpCode(addr);
        if (!e) {
            addr = "9999 Error: Illegal opcode; treated as 9999";
        } else {
            while (addr.size() < 4) {
                addr = "0" + addr;
            }
            int n = 0;
            for (int i = 1; i < addr.size(); ++i) {
                n = n * 10 + addr[i] - '0';
            }
            if (moduleAddr[mi] + n >= moduleAddr[mi + 1]) {
                char dest[4] = {0};
                stringstream ss;
                sprintf(dest, "%03d", moduleAddr[mi]);
                ss << dest;
                addr = string(1, addr[0]) + ss.str() +
                    " Error: Relative address exceeds module size; zero used";
            } else {
                char dest[4] = {0};
                stringstream ss;
                sprintf(dest, "%03d", n + moduleAddr[mi]);
                ss << dest;
                addr = string(1, addr[0]) + ss.str();
            }
        }
    } else if (c == "E") {
        bool e = checkOpCode(addr);

        if (!e) {
            addr = "9999 Error: Illegal opcode; treated as 9999";
        } else {
            while (addr.size() < 4) {
                addr = "0" + addr;
            }
            int n = 0;
            for (int i = 1; i < addr.size(); ++i) {
                n = n * 10 + addr[i] - '0';
            }
            if (n >= useSym[mi].size()) {
                addr += " Error: External address exceeds length of uselist; treated as immediate";
            } else {
                string desiredSymbol = useSym[mi][n];
                usedUseSym[desiredSymbol] = 1;
                if (symbolTable[desiredSymbol] == 0) {
                    addr += " Error: " + desiredSymbol +
                        " is not defined; zero used";
                } else {
                    usedDefineSym[desiredSymbol] = 1;
                    char dest[4] = {0};
                    stringstream ss;
                    sprintf(dest, "%03d", symbolTable[desiredSymbol] - 1);
                    ss << dest;
                    addr = string(1, addr[0]) + ss.str();
                }
            }
        }
    }
    return addr;
}

void printMemoryMap() {
    cout << "Memory Map" << endl;
    for (int i = 0; i < instr.size(); ++i) {
        map<string, int> usedUseSym;
        for (int j = 0; j < instr[i].size(); ++j) {
            Instruction in = instr[i][j];
            printf("%03d: ", moduleAddr[i] + j);
            cout << redirect(in, i, usedUseSym) << endl;
        }
        for (int j = 0; j < useSym[i].size(); ++j) {
            string s = useSym[i][j];
            if (usedUseSym[s] == 0) {
                stringstream ss;
                cout << "Warning: Module " << i + 1 << ": " << s <<
                    " appeared in the uselist but was not actually used" << endl;
            }
        }
    }
}

void printWarnings() {
    for (int i = 0; i < defineSym.size(); ++i) {
        for (int j = 0; j < defineSym[i].size(); ++j) {
            Symbol s= defineSym[i][j];
            if (usedDefineSym[s.name] == 0) {
                cout << "Warning: Module " << i + 1 << ": " << s.name <<
                    " was defined but never used" << endl;
            }
        }
    }
}

void generateResult() {
    printDefineSymbols();
    cout << endl;
    printMemoryMap();
    cout << endl;
    printWarnings();
    cout << endl;
}

int main(int argc, char* argv[]) {
     if (argc == 1) {
         cout << "Please enter the file name." << endl;
         return -1;
     } else {
         if (!freopen(argv[1], "r", stdin)) {
             cout << "Failed to open the file." << endl;
             return -1;
         }
     }
    tokenize();
    if (!parse()) {
        return -1;
    }
    //printParsedResults();
    generateResult();
    return 0;
}




