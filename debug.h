//
// Created by hhw on 2022/10/6.
//
// this file is using for debug
// 1. func to check status

#ifndef COMPILER_DEBUG_H
#define COMPILER_DEBUG_H
#include "Dependencies.h"
using namespace std;
void printOutputLexical() {
    int i;
    for(i = 0;i < outLexicalLine.size();i++) {
        printf("%s %s %d\n",outLexicalToken.at(i).c_str(),outLexicalWord.at(i).c_str(),outLexicalLine.at(i));
    }
}
void testFunc() {
    vector<identTable> symbolTable;
    vector<identTable> *symbolTmp;
    symbolTmp = &symbolTable;
    (*symbolTmp).emplace_back();
    (*symbolTmp).back().name = "a";
    printf("%s ",symbolTable.back().name.c_str());
}
void printFuncAndIdent() {
    vector<identTable>::iterator it;
    printf("UsualIdentTable\n");
    if(!UsualIdentTable.empty()) {
        for(it = UsualIdentTable.begin(); it != UsualIdentTable.end();it++) {
            printf("name: %s dimension: %d\n",it->name.c_str(),it->ident_kind);
            if(it->ident_kind == 0)
                printf("  = %d\n",it->value);
            if(it->ident_kind >= 1)
                printf("    one: %d ",it->dimension_one);
            if(it->ident_kind == 2)
                printf("two: %d\n",it->dimension_two);
            if(it->ident_kind >= 1) {
                auto el = it->arrayValue.begin();
                printf("    ");
                for(;el!=it->arrayValue.end();el++)
                    printf("%d ",*el);
                printf("\n");
            }
        }
    }
    printf("ConstIdentTable\n");
    if(!ConstIdentTable.empty()) {
        for(it = ConstIdentTable.begin(); it != ConstIdentTable.end();it++) {
            printf("name: %s dimension: %d",it->name.c_str(),it->ident_kind);
            if(it->ident_kind == 0)
                printf("  value: %d\n",it->value);
            else
                printf("\n");
            if(it->ident_kind >= 1)
                printf("    one: %d ",it->dimension_one);
            if(it->ident_kind == 2)
                printf("two: %d\n",it->dimension_two);
            if(it->ident_kind >= 1) {
                auto el = it->arrayValue.begin();
                printf("    ");
                for(;el!=it->arrayValue.end();el++)
                    printf("%d ",*el);
                printf("\n");
            }
        }
    }
    printf("FuncTable\n");
    if(!FuncTable.empty()) {
        auto el = FuncTable.begin();
        for(el = FuncTable.begin(); el != FuncTable.end();el++) {
            printf("name: %s params: %d  effective: %d\n",el->name.c_str(),el->params_num,el->effective);
            for(it = el->params_list.begin(); it != el->params_list.end();it++) {
                printf("    param: %s  dimension: %d\n",it->name.c_str(),it->ident_kind);
            }
        }
    }
}
void printSymbolTable() {
    auto it = allSymbolTable.begin();
    for(;it != allSymbolTable.end();it++) {
        vector<identTable> a = *it;
        auto el = a.begin();
        for(;el != a.end();el++)
            printf("%s \n",(*el).name.c_str());
        printf("newFunc\n");
    }
}
#endif //COMPILER_DEBUG_H
