//
// Created by hhw on 2022/10/13.
//

#ifndef COMPILER_ERRORHANDING_H
#define COMPILER_ERRORHANDING_H
#include <utility>

#include "Dependencies.h"
using namespace std;
bool ErrorMessage = false;
int NumberNeed(string inTest) {
    int ans = 0;
    auto it = inTest.begin();
    for(it = it + 1;it != inTest.end();it++) {
        if(*(it - 1) == '%' && *it == 'd') {
            ans++;
        }
    }
    return ans;
}
void ErrorHanding_a_string(string inTest, int line) {
    auto it = inTest.begin();
    for(it = it + 1;it != inTest.end();it++) {
        if(!isLegalChar(*it) || (*(it - 1) == '\\' && *it != 'n') || (*(it -1) == '%'&& *it != 'd') || (*it == '"' && it + 1 != inTest.end())) {
            if(ErrorMessage) printf("in line %d, you use undefined char in %s\n",line,inTest.c_str());
            printErrorHanding('a',line);
            break;
        }
    }
}
void ErrorHanding_b(int line) {
    if(ErrorMessage) printf("in line %d, there is a defined word ;\n",line);
    printErrorHanding('b',line);
}
void ErrorHanding_c(int line) {
    if(ErrorMessage) printf("in line %d, there is a undefined word ;\n",line);
    printErrorHanding('c',line);
}
void ErrorHanding_d(int line) {
    if(ErrorMessage) printf("in line %d, there is a error params number;\n",line);
    printErrorHanding('d',line);
}
void ErrorHanding_e(int line) {
    if(ErrorMessage) printf("in line %d, there is a error params;\n",line);
    printErrorHanding('e',line);
}
void ErrorHanding_f(int line) {
    if(ErrorMessage) printf("in line %d, there is a error return;\n",line);
    printErrorHanding('f',line);
}
void ErrorHanding_g(int line) {
    if(ErrorMessage) printf("in line %d, there need a return;\n",line);
    printErrorHanding('g',line);
}
void ErrorHanding_h(int line) {
    if(ErrorMessage) printf("in line %d, you are change the const;\n",line);
    printErrorHanding('h',line);
}
void ErrorHanding_i(int line) {
    if(ErrorMessage) printf("in line %d, there need a ;\n",line);
    printErrorHanding('i',line);
}
void ErrorHanding_j(int line) {
    if(ErrorMessage) printf("in line %d, there need a )\n",line);
    printErrorHanding('j',line);
}
void ErrorHanding_k(int line) {
    if(ErrorMessage) printf("in line %d, there need a ]\n",line);
    printErrorHanding('k',line);
}
void ErrorHanding_l(int line,string inTest, int nowHave) {
    int nowNeed = NumberNeed(std::move(inTest));    // move without copy
    if(nowNeed != nowHave) {
        if(ErrorMessage) printf("in line %d, the needed exp number is %d, now in fact is %d\n",line,nowNeed,nowHave);
        printErrorHanding('l',line);
    }
}
void ErrorHanding_m(int line) {
    if(ErrorMessage) printf("in line %d, extra break or continue\n",line);
    printErrorHanding('m',line);
}
bool CheckRepeatIdent(vector<identTable> a,const string& name) {
    if(a.empty())
        return true;
    auto it = a.begin();
    for(;it != a.end();it++) {
        if(it->name == name)
            return false;
    }
    return true;
}
bool CheckRepeatFunc(vector<funcTable> a,const string& name) {
    auto it = a.begin();
    for(;it != a.end();it++) {
        if(it->name == name)
            return false;
    }
    return true;
}
void GlobalConstTable(int name,int ident_ki) {
    if(CheckRepeatIdent(ConstIdentTable,outLexicalWord[name]) && CheckRepeatIdent(UsualIdentTable, outLexicalWord[name])) {
        ConstIdentTable.emplace_back();
        ConstIdentTable.back().name = outLexicalWord[name];
        ConstIdentTable.back().ident_kind = ident_ki;
        ConstIdentTable.back().changeable = false;
        ConstIdentTable.back().init = true;
        ConstIdentTable.back().isGlobal = true;
    }
    else
        ErrorHanding_b(outLexicalLine[name]);
}
void GlobalVarTable(int name,int ident_ki) {
    if(CheckRepeatIdent(UsualIdentTable,outLexicalWord[name]) && CheckRepeatIdent(ConstIdentTable,outLexicalWord[name])) {
        UsualIdentTable.emplace_back();
        UsualIdentTable.back().name = outLexicalWord[name];
        UsualIdentTable.back().ident_kind = ident_ki;
        UsualIdentTable.back().changeable = true;
        UsualIdentTable.back().isGlobal = true;
    }
    else
        ErrorHanding_b(outLexicalLine[name]);
}
void GlobalFuncTable(int name, int return_kind) {
    bool final = true; // 标记该项是否有用，若无用， 在完成函数处理后抛弃
    if( ! (CheckRepeatIdent(UsualIdentTable,outLexicalWord[name]) && CheckRepeatIdent(ConstIdentTable,outLexicalWord[name])
    && CheckRepeatFunc(FuncTable,outLexicalWord[name])) ) {
        ErrorHanding_b(outLexicalLine[name]);
        final = false;
    }
    FuncTable.emplace_back();
    FuncTable.back().name = outLexicalWord[name];
    FuncTable.back().return_kind = return_kind;
    FuncTable.back().effective = final;
}
void GlobalFuncParamsTable(int name, int ident_ki) {
    if(CheckRepeatIdent(FuncTable.back().params_list,outLexicalWord[name])) {
        FuncTable.back().params_list.emplace_back();
        FuncTable.back().params_list.back().name = outLexicalWord[name];
        FuncTable.back().params_list.back().ident_kind = ident_ki;
        FuncTable.back().params_num++;
    }
    else
        ErrorHanding_b(outLexicalLine[name]);
}
bool CheckSymbolTableRepeat(int name,const vector<identTable>& symbolTable,vector<identTable> funcParamsTable) {
    auto it = symbolTable.crbegin();
    for(;it != symbolTable.crend();it++) {
        int number;
        if(it->name == "}") {
            number = 1;
            it++;
           while(true) {
               if(it->name == "{")
                   number--;
               if(it->name == "}")
                   number++;
               if(number == 0)
                   break;
               it++;
           }
            continue;
        }
        if(it->name == "{" && (it+1) == symbolTable.crend())
            break;
        else if(it->name == "{")
            return true;
        if(it->name == outLexicalWord[name])
            return false;
    }
    auto el = funcParamsTable.begin();
    for(;el != funcParamsTable.end();el++) {
        if(el->name == outLexicalWord[name])
            return false;
    }
    return true;
}
void SymbolTable(int name, int ident_ki, bool change) {
    if(CheckSymbolTableRepeat(name,allSymbolTable.back(),FuncTable.back().params_list)) {
        (allSymbolTable.back()).emplace_back();
        (allSymbolTable.back()).back().name = outLexicalWord[name];
        (allSymbolTable.back()).back().ident_kind = ident_ki;
        (allSymbolTable.back()).back().changeable = change;
    }
    else
        ErrorHanding_b(outLexicalLine[name]);
}
identTable CheckDefineInIdent(int name) {
    int number;
    if(!allSymbolTable.empty()) {
        auto it = allSymbolTable.back().crbegin();
        for(;it != allSymbolTable.back().crend();it++) {
            if(it->name == "}") {
                number = 1;
                it++;
                while(true) {
                    if(it->name == "}")
                        number++;
                    if(it->name == "{")
                        number--;
                    if(number == 0)
                        break;
                    it++;
                }
                continue;
            }
            if(it->name == outLexicalWord[name]) {
                return *it;
            }
        }
    }
    if(!FuncTable.empty()) {
        auto el = FuncTable.back().params_list.begin();
        for(;el != FuncTable.back().params_list.end();el++) {
            if(el->name == outLexicalWord[name])
                return *el;
        }
    }
    auto el = UsualIdentTable.begin();
    for(;el != UsualIdentTable.end();el++) {
        if(el->name == outLexicalWord[name])
            return *el;
    }
    el = ConstIdentTable.begin();
    for(;el != ConstIdentTable.end();el++) {
        if(el->name == outLexicalWord[name])
            return *el;
    }
    return ErrorIdent;
}
funcTable CheckDefineInFunc(int name) {
    auto el = FuncTable.begin();
    for(;el != FuncTable.end();el++) {
        if(el->name == outLexicalWord[name])
            return *el;
    }
    return ErrorFunc;
}
bool inParamsList(const identTable& a) {
    auto el = FuncTable.back().params_list.begin();
    for(;el != FuncTable.back().params_list.end();el++) {
        if(el->name == a.name)
            return true;
    }
    return false;
}
#endif //COMPILER_ERRORHANDING_H
