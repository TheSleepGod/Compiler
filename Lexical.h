//
// Created by hhw on 2022/9/14.
//
/*  before division, we get a char first
 * we must keep this is same at ell times
 * at final we can't find a char for next word, which mean end;
 * return -1 : finish
 * return 0 : not finnish, not do
 * return 1: we get a number
 * return 2: we get an Ident
 * return 3: we get a oneChar
 * return 4: we get a String
 * return 5: we get a twoChar
 * */
#include "Dependencies.h"
#include "ErrorHanding.h"
#ifndef COMPILER_LEXICAL_H
#define COMPILER_LEXICAL_H
using namespace std;
char nowChar;
string wordTmp;
int line = 1;
int final = 0;
//read one char from input
bool reader() {
    if(scanf("%c",&nowChar) != EOF) {
        if(nowChar == '\r') {           // /r ==> /n
            scanf("%c",&nowChar);
        }
        // error: undefined char
        return true;
    }
    else {
        nowChar = '\0';
        final = 1;
        return false;
    }
}
int division() {
    if(final == 1) {
        return -1;
    }
    if(isdigit(nowChar)) { //this is number final
        while(isdigit(nowChar)) {
            wordTmp.push_back(nowChar);
            reader();
        }   //error 0 is beginning
        return 1;
    }
    else if(isalpha(nowChar) || nowChar == '_') { //final
        while(isIndent(nowChar)) {
            wordTmp.push_back(nowChar);
            reader();
        }
        return 2;
    }
    else if(isOneChar(nowChar)) {   //final
        wordTmp.push_back(nowChar);
        reader();
        return 3;
    }
    else if(nowChar == '/') {
        wordTmp.push_back(nowChar);
        reader();
        if(nowChar == '/') {// this is // ,we need find \n to leave there
            while(nowChar != '\n' && final != 1)
                reader();
            wordTmp.clear();
            return 0;
        }
        else if(nowChar == '*') { //this is /*, we need find */ to leave
            reader();
            while(true) {
                if(nowChar == '*') {
                    reader();
                    if(nowChar == '/') {
                        break;
                    }
                    else
                        continue;
                }
                if(nowChar == '\n')
                    line++;
                reader();
                if(final == 1)      //error we can't find a */ to march the /*
                    break;
            }
            wordTmp.clear();
            reader();
            return 0;
        }
        else //we find a /, this is a happy thing
            return 3;
    }
    else if(nowChar == '"') {   //final
        wordTmp.push_back(nowChar);
        reader();
        while(nowChar != '"') {
            if(nowChar == '\n')    // warning : this will make compiler think string with \n is right;
                line++;
            wordTmp.push_back(nowChar);
            reader();
        }
        wordTmp.push_back(nowChar);
        reader();
        return 4;
    }
    else if(nowChar == '|') {   //final
        wordTmp.push_back(nowChar);
        reader();
        if(nowChar == '|') {
            wordTmp.push_back(nowChar);
            reader();
            return 5;
        }
        // else ==> error | is not defined
    }
    else if(nowChar == '&') {   //final
        wordTmp.push_back(nowChar);
        reader();
        if(nowChar == '&') {
            wordTmp.push_back(nowChar);
            reader();
            return 5;
        }
        // else ==> error & is not defined
    }
    else if(nowChar == '<' || nowChar == '>' || nowChar == '=' || nowChar == '!') {
        wordTmp.push_back(nowChar);
        reader();
        if(nowChar == '=') {
            wordTmp.push_back(nowChar);
            reader();
            return 5;
        }
        else {
            return 3;
        }
    }
    else if(nowChar == '\n') {
        line++;
        reader();
        return 0;
    }
    else if(nowChar == ' ' || nowChar == '\t') {
        while(nowChar == ' ' || nowChar == '\t') {
            reader();
        }
        wordTmp.clear();
        return 0;
    }
    return -1;
}

void wordMemory() {
    bool IsReserved = false;
    auto first = wordTmp.begin();
    if(*first == '"') {
        addLexicalOutput("STRCON",wordTmp,line);
//        printf("STRCON %s\n",wordTmp.c_str());
        return;
    }
    else if(isdigit(*first)) {
        addLexicalOutput("INTCON",wordTmp,line);
//        printf("INTCON %s\n",wordTmp.c_str());
        return;
    }
    else {
        auto it = reservedWord.begin();
        for (;it != reservedWord.end(); it++) {
            if(wordTmp == it->first) {
                addLexicalOutput(reservedWord[it->first],wordTmp,line);
//                printf("%s %s\n",reservedWord[it->first].c_str(),wordTmp.c_str());
                IsReserved = true;
                break;
            }
        }
        if(IsReserved)
            return;
        if(isalpha(*first) || *first == '_') {
            addLexicalOutput("IDENFR",wordTmp,line);
//            printf("IDENFR %s\n",wordTmp.c_str());
        }
        //error !!!
        return;
    }

}
void Lexical() {
    ReservedInit();
    reader();
    while (division() != -1) {
//        printf("%d: ",line);   //we can use this to make sure the line;
        if(!wordTmp.empty()) {
            wordMemory();
        }
        wordTmp.clear();
    }
}
#endif //COMPILER_LEXICAL_H
