//
// Created by hhw on 2022/9/15.
//
// this file is the bottom of the compiler
// 1. the public var
// 2. all libs
// 3. IO and check func
#include <iostream>
#include <fstream>
#include <utility>
#include <vector>
#include <string>
#include <cstdio>
#include <map>
#include <vector>
#include <list>
#include <stack>
using namespace std;
#ifndef COMPILER_DEPENDENCIES_H
#define COMPILER_DEPENDENCIES_H
int AddExp();  // Exp -> Add -> Mul -> Unary -> Primary -> (Exp)
bool Block();   // Block -> {BlockItem} -> {Stmt} -> {Block}
bool SyntacticOutPut = false;   // control the output of SyntacticOutPut
bool ErrorHandingOutPut = true;    // control the output of ErrorHanding
bool QuaternionOutPut = true;

map<string,string> reservedWord;    // reserved word list
vector<string> outLexicalToken;      // output of Lexical
vector<string> outLexicalWord;      // output of Lexical
vector<int> outLexicalLine;         // output of Lexical
vector<string> data_string;
// now we need a Syntactic table
// Ident -> the Ident of it
// changeable -> const
// kind
// dimension value
// params return
// 结构体
// 函数 -》 返回值、参数列表
// 参数列表 -》 变量个数 变量维度 变量各维长度
// 变量 -》 变量类型  变量维度 变量各位长度 变量可变性
struct identTable {
    string name = "error!";
    int ident_kind = 0; // 0 is a / 1 is a[] / 2 is a[][] / -1 is bool
    int dimension_one = 0;
    int dimension_two = 0;
    int value = 0;
    bool init = false;
    bool changeable = true;
    bool isGlobal = false;
    vector<int> arrayValue;
    string reg = "NULL!";
    int fg_offset = -1;
} ErrorIdent;
struct funcTable {
    string name = "error!";
    int return_kind = 0;    // 0 is int / 1 is void
    int params_num = 0;
    vector<identTable> params_list; // in fact, wo only need keep the ident_kind is true
    bool effective = true;
} ErrorFunc;
struct st{
    int kind = 0;   // 0 is value 1 is string
    int value = 0;
    string name = "error!";
//    bool key = true;
    int offset = -1;
    string reg = "NULL!";
    int fg_offset = -1;
    bool melt = false;
    bool isAddress = false;
} ErrorSt;
vector<funcTable> FuncTable;    // the vector of func
vector<identTable> ConstIdentTable; // the vector of global var
vector<identTable> UsualIdentTable; // the vector of global const
vector<vector<identTable>> allSymbolTable;  //the table of func
stack<st> allStack;
string Int = "int";
string Const = "const";
string While = "while";
string Main = "main";
string Break = "break";
string Continue = "continue";
string If = "if";
string Else = "else";
string Getint = "getint";
string Printf = "printf";
string Return = "return";
string Void = "void";
string BitAnd = "bitand";
// Lexical.h
void ReservedInit() {
    reservedWord.insert(pair<string,string>(Int,"INTTK"));
    reservedWord.insert(pair<string,string>(Const,"CONSTTK"));
    reservedWord.insert(pair<string,string>(Main,"MAINTK"));
    reservedWord.insert(pair<string,string>(Break,"BREAKTK"));
    reservedWord.insert(pair<string,string>(Continue,"CONTINUETK"));
    reservedWord.insert(pair<string,string>(If,"IFTK"));
    reservedWord.insert(pair<string,string>(Else,"ELSETK"));
    reservedWord.insert(pair<string,string>(While,"WHILETK"));
    reservedWord.insert(pair<string,string>(Getint,"GETINTTK"));
    reservedWord.insert(pair<string,string>(Printf,"PRINTFTK"));
    reservedWord.insert(pair<string,string>(Return,"RETURNTK"));
    reservedWord.insert(pair<string,string>(Void,"VOIDTK"));
    reservedWord.insert(pair<string,string>(BitAnd,"BitAnd"));

    reservedWord.insert(pair<string,string>("!","NOT"));
    reservedWord.insert(pair<string,string>("&&","AND"));
    reservedWord.insert(pair<string,string>("||","OR"));
    reservedWord.insert(pair<string,string>("+","PLUS"));
    reservedWord.insert(pair<string,string>("-","MINU"));
    reservedWord.insert(pair<string,string>("*","MULT"));
    reservedWord.insert(pair<string,string>("/","DIV"));
    reservedWord.insert(pair<string,string>("%","MOD"));
    reservedWord.insert(pair<string,string>("<","LSS"));
    reservedWord.insert(pair<string,string>("<=","LEQ"));
    reservedWord.insert(pair<string,string>(">","GRE"));
    reservedWord.insert(pair<string,string>(">=","GEQ"));
    reservedWord.insert(pair<string,string>("==","EQL"));
    reservedWord.insert(pair<string,string>("!=","NEQ"));
    reservedWord.insert(pair<string,string>("=","ASSIGN"));
    reservedWord.insert(pair<string,string>(";","SEMICN"));
    reservedWord.insert(pair<string,string>(",","COMMA"));
    reservedWord.insert(pair<string,string>("(","LPARENT"));
    reservedWord.insert(pair<string,string>(")","RPARENT"));
    reservedWord.insert(pair<string,string>("[","LBRACK"));
    reservedWord.insert(pair<string,string>("]","RBRACK"));
    reservedWord.insert(pair<string,string>("{","LBRACE"));
    reservedWord.insert(pair<string,string>("}","RBRACE"));
}
void addLexicalOutput(const string& s1, const string& s2, const int line) { // add the token to the vector
    outLexicalToken.emplace_back(s1);
    outLexicalWord.emplace_back(s2);
    outLexicalLine.emplace_back(line);
}

bool isOneChar(char a) {
    if(a == '(' || a == ')'|| a == '{' || a == '}'
       || a == ';'|| a == '[' || a == ']' || a == ','
       || a == '+' || a == '-' || a == '*'|| a == '%')
        return true;
    else
        return false;
}

bool isIndent(char a) {
    return isalpha(a) || isdigit(a) || a == '_';
}
//  Syntactic.h
void printLexicalOutput(int now_pos) {
    if(SyntacticOutPut) {
        if(now_pos < outLexicalToken.size())
            printf("%s %s\n",outLexicalToken[now_pos].c_str(),outLexicalWord[now_pos].c_str());
        else
            printf("Warning : Dependencies.h 76");
    }
    else
        return;
}
void printLexicalExtra(const string& output) {
    if(SyntacticOutPut) printf("%s",output.c_str());
}
// ErrorHanding.h
bool isLegalChar(char a) {
        return a == 32 || a == 33 || (a >= 40 && a <= 126) || a == '"' || a == '%';
}
void printErrorHanding(char a,int line) {
    if(ErrorHandingOutPut) printf("%d %c\n",line,a);
}
// Mips.h
void printQuaternion(const string& str) {
    if(QuaternionOutPut) printf("%s\n",str.c_str());
}
bool code[5] = {true,true,true,true,true};
string getCode() {
    int i = 0;
    while(i < 5 && !code[i])
        i++;
    if(i == 5)
        printf("now all $ has been used");
    else if(i < 5)
        code[i] = false;
    return "$t" + to_string(i);
}
void ret(const string& a) {
    if(a == "$t0")
        code[0] = true;
    else if(a == "$t1")
        code[1] = true;
    else if(a == "$t2")
        code[2] = true;
    else if(a == "$t3")
        code[3] = true;
    else if(a == "$t4")
        code[4] = true;
}
bool checkCode(const string& reg) {
    return reg == "$t0" || reg == "$t1" || reg == "$t2" || reg == "$t3" || reg == "$t4";
}
#endif //COMPILER_DEPENDENCIES_H
