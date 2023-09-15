#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
//
// Created by hhw on 2022/9/20.
//
// we must keep at the beginning of Syntactic
// the word was the first of Syntactic
// we need move now_token to next at the end of Syntactic

// there has loop chain, the beginning of loop chain were defined in Dependencies

#ifndef COMPILER_SYNTACTIC_H
#define COMPILER_SYNTACTIC_H
#include <utility>

#include "Dependencies.h"
#include "Quaternion.h"
#include "Mips.h"
using namespace std;
int now_token = 0;
int cycleDepth = 0;
bool Global = true;
bool InMain = false;
int near_return_final = 0;
int init_array_position = 0;
int if_begin = 0;
vector<st> allGlobalGet;
stack<int> if_end;
int Exp() {
    int return_value;
    return_value = AddExp();
    printLexicalExtra("<Exp>\n");
    return return_value;
}
int LVal() {
    st tmp[3];
    int return_value;
    tmp[1] = ErrorSt;
    tmp[2] = ErrorSt;
    identTable TestTmp = CheckDefineInIdent(now_token);
    if(TestTmp.name == "error!")
        ErrorHanding_c(outLexicalLine[now_token]);
    return_value = TestTmp.ident_kind;
    printLexicalOutput(now_token++); //Ident
    while(outLexicalToken[now_token] == "LBRACK") {
        printLexicalOutput(now_token++);    //  [
        return_value--;
        Exp();
        tmp[2-return_value] = allStack.top();
        allStack.pop();
        if(outLexicalToken[now_token] != "RBRACK")
            ErrorHanding_k(outLexicalLine[now_token - 1]);
        else
            printLexicalOutput(now_token++); //]
    }
    if((Global || !TestTmp.changeable) && tmp[1].kind == 0 && tmp[2].kind == 0) {
        tmp[0].kind = 0;
        if(TestTmp.ident_kind == 0) {
            tmp[0].value = TestTmp.value;
        }
        else if(TestTmp.ident_kind == 1) {
            auto it = TestTmp.arrayValue.begin();
            it += tmp[2].value;
            tmp[0].value = *it;
        }
        else if(TestTmp.ident_kind == 2) {
            auto it = TestTmp.arrayValue.begin();
            it += tmp[1].value * TestTmp.dimension_two + tmp[2].value;
            tmp[0].value = *it;
        }
        allStack.push(tmp[0]);
    }
    else {
        // need change
        if(return_value != 0) { // this is a params, we need take it special
            ParamsArray(TestTmp,tmp,return_value);
        }
        else {
            if (TestTmp.ident_kind == 0) {
                tmp[0].kind = 1;
                tmp[0].name = TestTmp.name;
                tmp[0].offset = -1;
                tmp[0].fg_offset = TestTmp.fg_offset;
                tmp[0].reg = TestTmp.reg;
                allStack.push(tmp[0]);
            } else if (TestTmp.ident_kind == 1) {
                tmp[0].kind = 0;
                tmp[0].value = 4;
                allStack.push(tmp[0]);
                allStack.push(tmp[2]);
                operation("MULT");
                tmp[0] = GetArray(TestTmp);
                allStack.push(tmp[0]);
                operation("PLUS");
                allStack.top().isAddress = true;
            }
            else if(TestTmp.ident_kind == 2) {
                if(tmp[1].kind == 0) {
                    tmp[1].value = tmp[1].value * TestTmp.dimension_two * 4;
                    allStack.push(tmp[1]);
                }
                else {
                    tmp[0].kind = 0;
                    tmp[0].value = TestTmp.dimension_two * 4;
                    allStack.push(tmp[1]);
                    allStack.push(tmp[0]);
                    operation("MULT");
                }
                if(tmp[2].kind == 0) {
                    tmp[2].value *= 4;
                    allStack.push(tmp[2]);
                }
                else {
                    tmp[0].kind = 0;
                    tmp[0].value = 4;
                    allStack.push(tmp[2]);
                    allStack.push(tmp[0]);
                    operation("MULT");
                }
                if(tmp[1].kind == 0 && tmp[2].kind == 0) {
                    tmp[1].value = tmp[1].value + tmp[2].value;
                    allStack.pop();
                    allStack.pop();
                    allStack.push(tmp[1]);
                }
                else {
                    operation("PLUS");
                }
                tmp[0] = GetArray(TestTmp);
                allStack.push(tmp[0]);
                operation("PLUS");
                allStack.top().isAddress = true;
            }
        }
    }
    printLexicalExtra("<LVal>\n");
    return return_value;
}
int FuncRParams(funcTable funcTmp,int name) {
    int params_num = 1;
    int test_value;
    bool Error = true;
    st tmp;
    test_value = Exp();
    tmp = allStack.top();
    allStack.pop();

    if(tmp.kind == 0) {
        printf("push %d\n",tmp.value);
    }
    else if(tmp.kind == 1) {
        if(tmp.offset == -1)
            printf("push %s\n",tmp.name.c_str());
        else
            printf("push %s(%d)\n",tmp.name.c_str(),tmp.offset);
    }
    if(tmp.kind != 0 && checkCode(tmp.reg)) {
        st cache;
        cache.kind = 1;
        cache.reg = tmp.reg;
        cache.isAddress = tmp.isAddress;
        tmp.isAddress = false;
        tmp.reg = "NULL!";
        int i;
        bool undef = true;
        for(i = 0;i <= 9;i++) {
            if (!varReg[i].used) {
                tmp.reg = varReg[i].reg;
                varReg[i].name = tmp.name;
                varReg[i].used = true;
                undef = false;
                break;
            }
        }
        if(tmp.reg == "NULL!") {
            for(i = 1;i <= 3;i++) {
                if(!paramReg[i].used) {
                    tmp.reg = paramReg[i].reg;
                    paramReg[i].name = tmp.name;
                    paramReg[i].used = true;
                    undef = false;
                    break;
                }
            }
        }
        if(undef) {
            tmp.fg_offset = gpReg;
            gpReg += 4;
        }
        ParamsGet.push_back(tmp.reg);
        equalOperation(tmp,cache);
    }
    allStack.push(tmp);
    auto it = funcTmp.params_list.begin();
    if(funcTmp.params_num != 0 && it != funcTmp.params_list.end() && funcTmp.name != "error!" && it->ident_kind != test_value) {
        ErrorHanding_e(outLexicalLine[name]);
        Error = false;
    }
    it++;
    while(outLexicalToken[now_token] == "COMMA") {
        printLexicalOutput(now_token++);    // ,
        test_value = Exp();
        tmp = allStack.top();
        allStack.pop();
        if(tmp.kind == 0) {
            printf("push %d\n",tmp.value);
        }
        else if(tmp.kind == 1) {
            if(tmp.offset == -1)
                printf("push %s\n",tmp.name.c_str());
            else
                printf("push %s(%d)\n",tmp.name.c_str(),tmp.offset);
        }
        if(tmp.kind != 0 && checkCode(tmp.reg)) {
            st cache;
            cache.kind = 1;
            cache.reg = tmp.reg;
            cache.isAddress = tmp.isAddress;
            tmp.isAddress = false;
            tmp.reg = "NULL!";
            int i;
            bool undef = true;
            for(i = 0;i <= 9;i++) {
                if (!varReg[i].used) {
                    tmp.reg = varReg[i].reg;
                    varReg[i].name = tmp.name;
                    varReg[i].used = true;
                    undef = false;
                    break;
                }
            }
            if(tmp.reg == "NULL!") {
                for(i = 1;i <= 3;i++) {
                    if(!paramReg[i].used) {
                        tmp.reg = paramReg[i].reg;
                        paramReg[i].name = tmp.name;
                        paramReg[i].used = true;
                        undef = false;
                        break;
                    }
                }
            }
            if(undef) {
                tmp.fg_offset = gpReg;
                gpReg += 4;
            }
            ParamsGet.push_back(tmp.reg);
            equalOperation(tmp,cache);
        }
        allStack.push(tmp);
        params_num++;
        if(funcTmp.params_num != 0 && it != funcTmp.params_list.end() && funcTmp.name != "error!" && it->ident_kind != test_value && Error) {
            Error = false;
            ErrorHanding_e(outLexicalLine[name]);
        }
        it++;
    }
    printLexicalExtra("<FuncRParams>\n");
    return params_num;
}
int PrimaryExp() {
    st tmp;
    int return_value = 0;
    if(outLexicalToken[now_token] == "LPARENT") {
        printLexicalOutput(now_token++);   // (
        return_value = Exp();
        printLexicalOutput(now_token++);   // )
    }
    else if(outLexicalToken[now_token] == "INTCON") {
        tmp.kind = 0;
        tmp.value = StringToNumber(outLexicalWord[now_token]);
        allStack.push(tmp);
        printLexicalOutput(now_token++);
        printLexicalExtra("<Number>\n");
        return_value = 0;
    }
    else if(outLexicalToken[now_token] == "IDENFR") {
        return_value = LVal();
    }
    printLexicalExtra("<PrimaryExp>\n");
    return return_value;
}
int UnaryExp() {
    int opNum = 0;
    int params_number = 0;
    int name;
    int return_value;
    int minuNum = 0;
    int notNum = 0;
    while(outLexicalToken[now_token] == "PLUS" || outLexicalToken[now_token] == "MINU" || outLexicalToken[now_token] == "NOT") {
        if(outLexicalToken[now_token] == "MINU") {
            minuNum++;
        }
        else if(outLexicalToken[now_token] == "NOT") {
            notNum++;
        }
        printLexicalOutput(now_token++);
        printLexicalExtra("<UnaryOp>\n");
        opNum++;
    }
    if(outLexicalToken[now_token] == "IDENFR" && outLexicalToken[now_token + 1] == "LPARENT") {
        st tmp;
        funcTable TestTmp = CheckDefineInFunc(now_token);
        if(TestTmp.name == "error!")
            ErrorHanding_c(outLexicalLine[now_token]);
        name = now_token;
        printLexicalOutput(now_token++);    //ident
        printLexicalOutput(now_token++);    //(
        if(outLexicalToken[now_token] != "RPARENT" && outLexicalLine[now_token - 1] == outLexicalLine[now_token] && outLexicalToken[now_token] != "SEMICN") {//no params
            params_number = FuncRParams(TestTmp,name);
        }
        if(params_number != TestTmp.params_num && TestTmp.name != "error!") {
            ErrorHanding_d(outLexicalLine[name]);
        }
        if(outLexicalToken[now_token] != "RPARENT")
            ErrorHanding_j(outLexicalLine[now_token - 1]);
        else {
            printLexicalOutput(now_token++);    // )
        }
        int sw;
        int now_fp = gpReg;
        moveGp(now_fp,TestTmp.params_num);
        sw = saveReg();
        fprintf(fh,"addi $fp $fp %d\n",now_fp);
        fprintf(fh,"jal %s\n",TestTmp.name.c_str());
        fprintf(fh,"addi $fp $fp %d\n",-now_fp);
        loadReg(sw);
        tmp.name = "ret";
        tmp.kind = 1;
        tmp.offset = -1;
        tmp.reg = "$v0";
        st func;
        bool undef = true;
        func.name = "func";
        func.melt = true;
        func.kind = 1;
        int i;
        for(i = 0;i <= 9;i++) {
            if (!varReg[i].used) {
                func.reg = varReg[i].reg;
                varReg[i].name = "func";
                varReg[i].used = true;
                undef = false;
                break;
            }
        }
        if(undef) {
            func.fg_offset = gpReg;
            gpReg += 4;
        }
        equalOperation(func,tmp);
        allStack.push(func);
        return_value = TestTmp.return_kind;
    }
    else if(outLexicalToken[now_token] == "LPARENT" || outLexicalToken[now_token] == "INTCON" || outLexicalToken[now_token] == "IDENFR") {
        return_value = PrimaryExp();
    }
    while(opNum > 0) {
        printLexicalExtra("<UnaryExp>\n");
        opNum--;
    }
    if(minuNum % 2 != 0) {
        if(allStack.top().kind == 0) {
            allStack.top().value = -allStack.top().value;
        }
        else {
            st a = allStack.top();
            allStack.pop();
            allStack.push(negReg(a));
        }
    }
    if(notNum != 0) {
        if(notNum % 2 == 0) {
            if(allStack.top().kind == 0) {
                if(allStack.top().value != 0)
                    allStack.top().value = 1;
                else
                    allStack.top().value = 0;
            }
            else {
                st a = allStack.top();
                allStack.pop();
                allStack.push(yesReg(a));
            }
        }
        else {
            if(allStack.top().kind == 0) {
                if(allStack.top().value != 0)
                    allStack.top().value = 0;
                else
                    allStack.top().value = 1;
            }
            else {
                st a = allStack.top();
                allStack.pop();
                allStack.push(notReg(a));
            }
        }
    }
    printLexicalExtra("<UnaryExp>\n");
    return return_value;
}
int MulExp() {
    int return_value;
    string doing;
    return_value = UnaryExp();
    while(outLexicalToken[now_token] == "MULT" || outLexicalToken[now_token] == "DIV" || outLexicalToken[now_token] == "MOD" || outLexicalToken[now_token] == "BitAnd") {
        printLexicalExtra("<MulExp>\n");
        doing = outLexicalToken[now_token];
        printLexicalOutput(now_token++);
        UnaryExp();
        operation(doing);
    }
    printLexicalExtra("<MulExp>\n");
    return return_value;
}
int AddExp() {
    int return_value;
    string doing;
    return_value = MulExp();
    printLexicalExtra("<AddExp>\n");
    while(outLexicalToken[now_token] == "PLUS" || outLexicalToken[now_token] == "MINU") {
        doing = outLexicalToken[now_token];
        printLexicalOutput(now_token++);
        MulExp();
        operation(doing);
        printLexicalExtra("<AddExp>\n");
    }
    return return_value;
}
bool ConstExp() {
    AddExp();
    printLexicalExtra("<ConstExp>\n");
    return true;
}
bool ConstInitVal() {
    if(outLexicalToken[now_token] != "LBRACE") {
        ConstExp();
        st tmp = allStack.top();
        allStack.pop();
        if(Global && ConstIdentTable.back().ident_kind  ==  0)
            ConstIdentTable.back().value = tmp.value;
        else if(Global && ConstIdentTable.back().ident_kind != 0)
            ConstIdentTable.back().arrayValue.emplace_back(tmp.value);
        else if(!Global && allSymbolTable.back().back().ident_kind == 0)
            allSymbolTable.back().back().value = tmp.value;
        else if(!Global && allSymbolTable.back().back().ident_kind != 0)
            allSymbolTable.back().back().arrayValue.emplace_back(tmp.value);
    }
    else {
        printLexicalOutput(now_token++); // {
        if(outLexicalToken[now_token] == "RBRACE") {        //maybe error : in extra rule, this is undefined
            printLexicalOutput(now_token++); // }
            printLexicalExtra("<ConstInitVal>\n");
            return false;
        }
        ConstInitVal();
        while(outLexicalToken[now_token] == "COMMA") {
            printLexicalOutput(now_token++);
            ConstInitVal();
        }
        printLexicalOutput(now_token++); // }
    }
    printLexicalExtra("<ConstInitVal>\n");
    return true;
}
bool ConstDef() {
    int Ident_di = 0;
    int name = now_token;
    bool newConstKey = false;
    st tmp[3];
    printLexicalOutput(now_token++);    //Ident
    while(outLexicalToken[now_token] == "LBRACK") {
        Ident_di++;
        printLexicalOutput(now_token++); // [
        ConstExp();
        tmp[Ident_di] = allStack.top();
        allStack.pop();
        if(outLexicalToken[now_token] != "RBRACK")
            ErrorHanding_k(outLexicalLine[now_token - 1]);
        else
            printLexicalOutput(now_token++); //]
    }
    if(Global) {
        GlobalConstTable(name,Ident_di);
        if(Ident_di >= 1)
            ConstIdentTable.back().dimension_one = tmp[1].value;
        if(Ident_di == 2)
            ConstIdentTable.back().dimension_two = tmp[2].value;
        printDef(ConstIdentTable.back());
        if(ConstIdentTable.back().ident_kind != 0)
            newConstKey = true;
    }
    else {
        SymbolTable(name,Ident_di, false);
        if(Ident_di >= 1)
            allSymbolTable.back().back().dimension_one = tmp[1].value;
        if(Ident_di == 2)
            allSymbolTable.back().back().dimension_two = tmp[2].value;
        printDef(allSymbolTable.back().back());
    }
    printLexicalOutput(now_token++);  // =
    ConstInitVal();
    if(newConstKey)
        newGlobalVar(ConstIdentTable.back());
    printLexicalExtra("<ConstDef>\n");
    return true;
}
bool ConstDecl() {
    printLexicalOutput(now_token++);    //const
    printLexicalOutput(now_token++);    // int
    ConstDef();
    while(outLexicalToken[now_token] == "COMMA") {
        printLexicalOutput(now_token++);    //,
        ConstDef();
    }
    if(outLexicalToken[now_token] != "SEMICN")
        ErrorHanding_i(outLexicalLine[now_token - 1]);
    else
        printLexicalOutput(now_token++);    //;
    printLexicalExtra("<ConstDecl>\n");
    return true;
}
bool InitVal() {
    st tmp;
    if(outLexicalToken[now_token] == "LBRACE") {
        printLexicalOutput(now_token++);    // {
        if(outLexicalToken[now_token] == "RBRACE") {
            printLexicalOutput(now_token++);
            printLexicalExtra("<InitVal>\n");
            return true;
        }
        InitVal();
        while(outLexicalToken[now_token] == "COMMA") {
            printLexicalOutput(now_token++);    // ,
            InitVal();
        }
        printLexicalOutput(now_token++);    // }
    }
    else {
        Exp();
        tmp = allStack.top();
        allStack.pop();
        if(Global && UsualIdentTable.back().ident_kind == 0)
            UsualIdentTable.back().value = tmp.value;
        else if(Global && UsualIdentTable.back().ident_kind != 0) {
            UsualIdentTable.back().arrayValue.emplace_back(tmp.value);
        }
        else if(!Global && allSymbolTable.back().back().init) {
            if(tmp.kind == 0) {
                if(allSymbolTable.back().back().ident_kind == 0) {
                    printVarDef(allSymbolTable.back().back(),tmp.value,0);
                    allSymbolTable.back().back().value = tmp.value;
                }
                else {
                    printVarDef(allSymbolTable.back().back(),tmp.value,init_array_position);
                    allSymbolTable.back().back().arrayValue.emplace_back(tmp.value);
                    init_array_position += 1;
                }
            }
            else if(tmp.kind == 1) {
                st tt;
                if(allSymbolTable.back().back().ident_kind == 0) {
                    tt.kind = 1;
                    tt.name = allSymbolTable.back().back().name;
                    tt.reg = allSymbolTable.back().back().reg;
                    tt.fg_offset = allSymbolTable.back().back().fg_offset;
                    equalOperation(tt,tmp);
                    ret(tmp.name);
                }
                else {
                    tt.kind = 1;
                    tt.name = allSymbolTable.back().back().name;
                    tt.fg_offset = allSymbolTable.back().back().fg_offset + init_array_position * 4;
                    printf("%s[%d] = %s\n",tt.name.c_str(),init_array_position,tmp.name.c_str());
                    init_array_position += 1;
                    equalOp(tt,tmp);
                    ret(tmp.name);
                }
            }
        }
    }
    printLexicalExtra("<InitVal>\n");
    return true;
}
bool varDef() {
    int Ident_di = 0;
    int name = now_token;
    st tmp[3];
    if(outLexicalToken[now_token] == "IDENFR") {    // else, the varDef should exit
        printLexicalOutput(now_token++);    // a
        while(outLexicalToken[now_token] == "LBRACK") {
            Ident_di++;
            printLexicalOutput(now_token++);    // [
            ConstExp();
            tmp[Ident_di] = allStack.top();
            allStack.pop();
            if(outLexicalToken[now_token] != "RBRACK")
                ErrorHanding_k(outLexicalLine[now_token - 1]);
            else
                printLexicalOutput(now_token++); //]
        }
        if(Global) {
            GlobalVarTable(name,Ident_di);
            if(Ident_di >= 1)
                UsualIdentTable.back().dimension_one = tmp[1].value;
            if(Ident_di == 2)
                UsualIdentTable.back().dimension_two = tmp[2].value;
            printDef(UsualIdentTable.back());
        }
        else {
            SymbolTable(name,Ident_di, true);
            if(Ident_di >= 1)
                allSymbolTable.back().back().dimension_one = tmp[1].value;
            if(Ident_di == 2)
                allSymbolTable.back().back().dimension_two = tmp[2].value;
            if(Ident_di == 0)
                getReg();
            else
                getArrayReg(allSymbolTable.back().back().dimension_one,allSymbolTable.back().back().dimension_two);
            printDef(allSymbolTable.back().back());
        }
        if(outLexicalToken[now_token] == "ASSIGN") {
            printLexicalOutput(now_token++);    // =
            if(outLexicalToken[now_token] != "GETINTTK") {
                init_array_position = 0;
                if(Global)
                    UsualIdentTable.back().init = true;
                else
                    allSymbolTable.back().back().init = true;
                InitVal();
            }
            else {
                init_array_position = 0;
                if(!Global) {
                    printLexicalOutput(now_token++);    // getint
                    printLexicalOutput(now_token++);    // (
                    printLexicalOutput(now_token++);    // )
                    st tmpp;
                    tmpp.reg = allSymbolTable.back().back().reg;
                    tmpp.kind = 1;
                    tmpp.name = allSymbolTable.back().back().name;
                    tmpp.fg_offset = allSymbolTable.back().back().fg_offset;
                    allSymbolTable.back().back().init = true;
                    getInt(tmpp);
                }
                else {
                    st tmpp;
                    tmpp.kind = 1;
                    tmpp.name = outLexicalWord[name];
                    UsualIdentTable.back().init = true;
                    allGlobalGet.push_back(tmpp);
                }
            }
        }
        if(!UsualIdentTable.empty() && Global && !UsualIdentTable.back().init) {    // 全局变量初始化
            if(Ident_di == 0) {
                UsualIdentTable.back().value = 0;
            }
            else if(Ident_di == 1) {
                int i = 0;
                for(;i < UsualIdentTable.back().dimension_one;i++)
                    UsualIdentTable.back().arrayValue.emplace_back(0);
            }
            else if(Ident_di == 2) {
                int i = 0;
                for(;i < UsualIdentTable.back().dimension_one * UsualIdentTable.back().dimension_two;i++)
                    UsualIdentTable.back().arrayValue.emplace_back(0);
            }
        }
        if(Global)
            newGlobalVar(UsualIdentTable.back());
    }
    printLexicalExtra("<VarDef>\n");
    return true;
}
bool VarDecl() {
    printLexicalOutput(now_token++);    // int
    varDef();
    while(outLexicalToken[now_token] == "COMMA") {
        printLexicalOutput(now_token++);    // ,
        varDef();
    }
    if(outLexicalToken[now_token] != "SEMICN")
        ErrorHanding_i(outLexicalLine[now_token - 1]);
    else
        printLexicalOutput(now_token++);
    printLexicalExtra("<VarDecl>\n");
    return true;
}
bool Decl() {
    if(outLexicalToken[now_token] == "CONSTTK") {
        ConstDecl();
        return true;
    }
    else if(outLexicalToken[now_token + 2] != "LPARENT") {
        VarDecl();
        return true;
    }
    return false;
}
bool RelExp(const string& if_ok,const string& if_no) {
    AddExp();
    int i;
    bool fM = false;
    printLexicalExtra("<RelExp>\n");
    while(outLexicalToken[now_token] == "LSS" || outLexicalToken[now_token] == "LEQ" || outLexicalToken[now_token] == "GRE" || outLexicalToken[now_token] == "GEQ") {
        i = now_token;
        printLexicalOutput(now_token++);    // > < >= <=
        AddExp();
        fM = true;
        compare(outLexicalToken[i],if_ok,if_no);
        printLexicalExtra("<RelExp>\n");
    }
    if(!fM) {
        st a = allStack.top();
        allStack.pop();
        if(a.kind != 0 && *a.reg.begin() != '$' && a.fg_offset == -1) {
            a.reg = getCode();
            fprintf(fh,"lw %s %s\n",a.reg.c_str(),a.name.c_str());
        }
        else if(a.kind != 0 && *a.reg.begin() != '$' && a.fg_offset != -1) {
            a.reg = getCode();
            fprintf(fh,"lw %s %d($fp)\n",a.reg.c_str(),a.fg_offset);
        }
        allStack.emplace(a);
    }
    return true;
}
bool EqExp(const string& if_ok,const string& if_no) {
    RelExp(if_ok,if_no);
    printLexicalExtra("<EqExp>\n");
    int i;
    while(outLexicalToken[now_token] == "EQL" || outLexicalToken[now_token] == "NEQ") {
        i = now_token;
        printLexicalOutput(now_token++);    // != ==
        RelExp(if_ok,if_no);
        compare(outLexicalToken[i],if_ok,if_no);
        printLexicalExtra("<EqExp>\n");
    }
    if(allStack.top().kind == 0) {
        if(allStack.top().value != 0)
            fprintf(fh,"j %s\n",if_ok.c_str());
        else
            fprintf(fh,"j %s\n",if_no.c_str());
    }
    else {
        fprintf(fh,"beqz %s %s\n",allStack.top().reg.c_str(),if_no.c_str());
        fprintf(fh,"j %s\n",if_ok.c_str());
    }
    ret(allStack.top().reg);
    allStack.pop();
    return true;
}
int EqNum = 0;
bool LandExp(const string& if_ok,const string& if_no) {
    string Land_ok = "Eq_" + to_string(EqNum);
    EqNum++;
    EqExp(Land_ok,if_no);
    printLexicalExtra("<LAndExp>\n");
    while(outLexicalToken[now_token] == "AND") {
        fprintf(fh,"%s:\n",Land_ok.c_str());
        printLexicalOutput(now_token++);    // &&
        Land_ok = "Eq_" + to_string(EqNum);
        EqNum++;
        EqExp(Land_ok,if_no);
        printLexicalExtra("<LAndExp>\n");
    }
    fprintf(fh,"%s:\n",Land_ok.c_str());
    fprintf(fh,"j %s\n",if_ok.c_str());
    return true;
}
int LandNum = 0;
bool LOrExp(const string& if_ok,const string& if_no) {
    string Lor_no = "Lor_" + to_string(LandNum);
    LandNum++;
    LandExp(if_ok,Lor_no);
    printLexicalExtra("<LOrExp>\n");
    while(outLexicalToken[now_token] == "OR") {
        printLexicalOutput(now_token++);    // ||
        fprintf(fh,"%s:\n",Lor_no.c_str());
        Lor_no = "Lor_" + to_string(LandNum);
        LandNum++;
        LandExp(if_ok,Lor_no);
        printLexicalExtra("<LOrExp>\n");
    }
    fprintf(fh,"%s:\n",Lor_no.c_str());
    fprintf(fh,"j %s\n",if_no.c_str());
    return true;
}
bool FormatString() {
    ErrorHanding_a_string(outLexicalWord[now_token],outLexicalLine[now_token]);
    printLexicalOutput(now_token++);    //string
    return true;
}
bool Cond(const string& if_ok,const string& if_no) {
    LOrExp(if_ok,if_no);
    printLexicalExtra("<Cond>\n");
    return true;
}
int WhileNum = 0;
stack<string> break_begin;
stack<string> break_final;
bool Stmt() {
    ret("$t0");
    ret("$t1");
    ret("$t2");
    ret("$t3");
    ret("$t4");
    st quaternion[3];
    if(outLexicalToken[now_token] == "LBRACE") {
        Block();
    }
    else if(outLexicalToken[now_token] == "IFTK") {
        printLexicalOutput(now_token++); // if
        printLexicalOutput(now_token++);    // (
        Cond("if_begin_" + to_string(if_begin),"if_else_" + to_string(if_begin));
        if(outLexicalToken[now_token] != "RPARENT")
            ErrorHanding_j(outLexicalLine[now_token - 1]);
        else
            printLexicalOutput(now_token++);    // )
        if_end.emplace(if_begin);
        fprintf(fh,"if_begin_%d:\n",if_begin);
        if_begin++;
        Stmt();
        if(outLexicalToken[now_token] == "ELSETK") {
            fprintf(fh,"j if_end_%d\n",if_end.top());
            fprintf(fh,"if_else_%d:\n",if_end.top());
            printLexicalOutput(now_token++);    // else
            Stmt();
            fprintf(fh,"if_end_%d:\n",if_end.top());
        }
        else {
            fprintf(fh,"if_else_%d:\n",if_end.top());
        }
        if_end.pop();
    }
    else if(outLexicalToken[now_token] == "WHILETK") {
        cycleDepth++;
        printLexicalOutput(now_token++); //while
        printLexicalOutput(now_token++); // (
        string WhileLab = "while__" + to_string(WhileNum);
        fprintf(fh,"%s:\n",WhileLab.c_str());
        WhileNum++;
        Cond("if_begin_" + to_string(if_begin),"if_end_" + to_string(if_begin));
        if(outLexicalToken[now_token] != "RPARENT")
            ErrorHanding_j(outLexicalLine[now_token - 1]);
        else
            printLexicalOutput(now_token++);    // )
        if_end.emplace(if_begin);
        fprintf(fh,"if_begin_%d:\n",if_begin);
        break_begin.emplace(WhileLab);
        break_final.emplace("if_end_" + to_string(if_end.top()));
        if_begin++;
        Stmt();
        fprintf(fh,"j %s\n",WhileLab.c_str());
        fprintf(fh,"if_end_%d:\n",if_end.top());
        break_begin.pop();
        break_final.pop();
        if_end.pop();
        cycleDepth--;
    }
    else if(outLexicalToken[now_token] == "BREAKTK" || outLexicalToken[now_token] == "CONTINUETK") {
        if(outLexicalToken[now_token] == "BREAKTK") {
            fprintf(fh,"j %s\n",break_final.top().c_str());
        }
        else {
            fprintf(fh,"j %s\n",break_begin.top().c_str());
        }
        printLexicalOutput(now_token++);    // break or continue
        if(outLexicalToken[now_token] != "SEMICN")
            ErrorHanding_i(outLexicalLine[now_token - 1]);
        else
            printLexicalOutput(now_token++);    // ;
        if(cycleDepth == 0)
            ErrorHanding_m(outLexicalLine[now_token - 1]);
    }
    else if(outLexicalToken[now_token] == "RETURNTK") {
        int name = now_token;
        printLexicalOutput(now_token++);    // return
        if(outLexicalToken[now_token] == "RBRACE") {
            ErrorHanding_i(outLexicalLine[now_token - 1]);
            near_return_final = now_token - 1;
        }
        else {
            bool HasExp = false;
            if(outLexicalToken[now_token] != "SEMICN" && outLexicalLine[now_token - 1] == outLexicalLine[now_token]) {
                Exp();
                st tmp = allStack.top();
                allStack.pop();
                if(tmp.reg != "$v0") {
                    setReturn(tmp);
                }
                if(tmp.kind == 0) {
                    printf("ret %d\n",tmp.value);
                }
                else {
                    if(tmp.offset != -1)
                        printf("ret %s(%d)\n",tmp.name.c_str(),tmp.offset);
                    else
                        printf("ret %s\n",tmp.name.c_str());
                }
                HasExp = true;
            }
            near_return_final = now_token;
            if(FuncTable.back().return_kind == 1 && HasExp)
                ErrorHanding_f(outLexicalLine[name]);
            if(outLexicalToken[now_token] != "SEMICN")
                ErrorHanding_i(outLexicalLine[now_token - 1]);
            else
                printLexicalOutput(now_token++);    // ;
        }
        if(!InMain)
            funcReturn();
        else
            mainReturn();
    }
    else if(outLexicalToken[now_token] == "PRINTFTK") {
        printLexicalOutput(now_token++); // printf
        printLexicalOutput(now_token++); // (
        FormatString();
        int NowHave = 0;
        vector<st> all;
        st tmp;
        string split;
        string FormatStr = outLexicalWord[now_token - 1];
        while(outLexicalToken[now_token] == "COMMA") {
            printLexicalOutput(now_token++); // ,
            Exp();
            int i = 0;
            bool def = false;
            tmp.kind = 1;
            for(;i <= 9;i++) {
                if (!varReg[i].used) {
                    tmp.reg = varReg[i].reg;
                    varReg[i].used = true;
                    def = true;
                }
            }
            if(!def) {
                tmp.reg = "NULL!";
                tmp.fg_offset = gpReg;
                gpReg += 4;
            }
            st p = allStack.top();
            allStack.pop();
            equalOperation(tmp,p);
            all.push_back(tmp);
            NowHave++;
        }
        auto it = FormatStr.begin() + 1;
        while(*it != '%' && it + 1 != FormatStr.end()) {
            split.push_back(*it);
            it++;
        }
        if(!split.empty()) {
            data_string.emplace_back(split);
            printf("print str_%zu\n",data_string.size());
            printStr((int )data_string.size());
            split.clear();
        }
        if(*it == '%') {
            it++;  it++; // %d
        }
        auto lk = all.begin();
        while(!all.empty() && lk != all.end()) {
            tmp = *lk;
            relaxReg(tmp.reg);
            lk++;
            if(tmp.kind == 0) {
                printf("print %d\n",tmp.value);
                printNum(tmp.value);
            }
            else {
                printf("print %d %s\n",tmp.offset,tmp.name.c_str());
                if(tmp.offset == -1) {
                    printVar(tmp);
                }
            }
            while(*it != '%' && it + 1 != FormatStr.end()) {
                split.push_back(*it);
                it++;
            }
            if(!split.empty()) {
                data_string.emplace_back(split);
                printf("print str_%zu\n",data_string.size());
                printStr((int )data_string.size());
                split.clear();
            }
            if(*it == '%') {
                it++;  it++; // %d
            }
        }
        all.clear();
        ErrorHanding_l(outLexicalLine[now_token - 1],FormatStr,NowHave);
        if(outLexicalToken[now_token] != "RPARENT")
            ErrorHanding_j(outLexicalLine[now_token - 1]);
        else
            printLexicalOutput(now_token++);    // )
        if(outLexicalToken[now_token] != "SEMICN")
            ErrorHanding_i(outLexicalLine[now_token - 1]);
        else
            printLexicalOutput(now_token++);    // ;
    }
    else {
        bool isExp = true;
        int i = now_token;
        while (i < outLexicalToken.size() - 1 && outLexicalToken[i] != "SEMICN" && outLexicalLine[i] == outLexicalLine[i + 1]) {
            if(outLexicalToken[i] == "ASSIGN") {
                isExp = false;
                break;
            }
            i++;
        }
        if(isExp) {
            if(outLexicalToken[now_token] != "SEMICN")
                Exp();
            if(outLexicalToken[now_token] != "SEMICN")
                ErrorHanding_i(outLexicalLine[now_token - 1]);
            else
                printLexicalOutput(now_token++); // ;
        }
        else {
            identTable tmp = CheckDefineInIdent(now_token);
            if(!tmp.changeable)
                ErrorHanding_h(outLexicalLine[now_token]);
            LVal();
            printLexicalOutput(now_token++);    // =
            if(outLexicalToken[now_token] == "GETINTTK") {
                printLexicalOutput(now_token++); // getint
                printLexicalOutput(now_token++); // (
                if(outLexicalToken[now_token] != "RPARENT")
                    ErrorHanding_j(outLexicalLine[now_token - 1]);
                else
                    printLexicalOutput(now_token++);    // )
                quaternion[1] = allStack.top();
                allStack.pop();
                getInt(quaternion[1]);
                printf("scanf t6\n");
                printf("%s = t6\n",quaternion[1].name.c_str());
                ret(quaternion[1].name);
            }
            else {
                Exp();
                quaternion[2] = allStack.top();
                allStack.pop();
                quaternion[1] = allStack.top();
                allStack.pop();
                equalOperation(quaternion[1],quaternion[2]);
                ret(quaternion[2].name);
            }
            if(outLexicalToken[now_token] != "SEMICN")
                ErrorHanding_i(outLexicalLine[now_token - 1]);
            else
                printLexicalOutput(now_token++); // ;
        }
    }
    printLexicalExtra("<Stmt>\n");
    return true;
}
bool BlockItem() {
    if(outLexicalToken[now_token] == "CONSTTK" || outLexicalToken[now_token] == "INTTK")
        Decl();
    else
        Stmt();
    return true;
}
bool Block() {
    (allSymbolTable.back()).emplace_back();
    (allSymbolTable.back()).back().name = "{";
//    printf("%s \n",(allSymbolTable.back()).back().name.c_str());
    printLexicalOutput(now_token++); // {
    while(outLexicalToken[now_token] != "RBRACE")
        BlockItem();
    (allSymbolTable.back()).emplace_back();
    (allSymbolTable.back()).back().name = "}";
    auto it = allSymbolTable.back().crbegin();
    for(;it != allSymbolTable.back().crend();it++) {
        if(it->name == "{")
            break;
        ret(it->reg);
        ret(it->name);
        relaxReg(it->reg);
    }
    printLexicalOutput(now_token++);    // }
    printLexicalExtra("<Block>\n");
    return true;
}
#pragma clang diagnostic pop
bool FuncFParam() {
    st tmp;
    int Number_di = 0;
    int name;
    printLexicalOutput(now_token++); // int
    name = now_token;
    printLexicalOutput(now_token++); // Ident
    if(outLexicalToken[now_token] == "LBRACK") {    // has one
        Number_di = 1;
        printLexicalOutput(now_token++);    // [
        if(outLexicalToken[now_token] != "RBRACK")
            ErrorHanding_k(outLexicalLine[now_token - 1]);
        else
            printLexicalOutput(now_token++); //]
    }
    if(outLexicalToken[now_token] == "LBRACK")  {    // has two []
        Number_di = 2;
        printLexicalOutput(now_token++);    // [
        ConstExp();
        if(outLexicalToken[now_token] != "RBRACK")
            ErrorHanding_k(outLexicalLine[now_token - 1]);
        else
            printLexicalOutput(now_token++); //]
    }
    GlobalFuncParamsTable(name,Number_di);
    if(Number_di == 2) {
        tmp = allStack.top();
        allStack.pop();
        FuncTable.back().params_list.back().dimension_two = tmp.value;
    }
    FuncTable.back().params_list.back().fg_offset = gpReg;
    gpReg += 4;
    printLexicalExtra("<FuncFParam>\n");
    return true;
}
bool FuncFParams() {
    int params_num = 1;
    FuncFParam();
    while(outLexicalToken[now_token] == "COMMA") {
        printLexicalOutput(now_token++);    // ,
        FuncFParam();
        params_num++;
    }
    printLexicalExtra("<FuncFParams>\n");
    return true;
}
bool Func() {
    if(outLexicalToken[now_token + 1] == "MAINTK") {
        return false;
    }
    int return_kind;
    int name;
    vector<identTable> symbolTable;
    allSymbolTable.emplace_back(symbolTable);
    if(outLexicalToken[now_token] == "VOIDTK" || outLexicalToken[now_token] == "INTTK") {
        if(outLexicalToken[now_token] == "VOIDTK")
            return_kind = 1;
        else
            return_kind = 0;
        printLexicalOutput(now_token++);    // int or void
        printLexicalExtra("<FuncType>\n");
        name = now_token;
        newFunc(outLexicalWord[name]);
        printLexicalOutput(now_token++);    // ident
        printLexicalOutput(now_token++);    // (
        GlobalFuncTable(name,return_kind);
        if(outLexicalToken[now_token] != "RPARENT" && outLexicalToken[now_token] != "LBRACE") {
            FuncFParams();      // in there, we sure the func has params
        }
        if(outLexicalToken[now_token] != "RPARENT")
            ErrorHanding_j(outLexicalLine[now_token - 1]);
        else
            printLexicalOutput(now_token++);    // )
        if(QuaternionOutPut) {
            printf("%s %s()\n",FuncTable.back().return_kind == 1?"void":"int",FuncTable.back().name.c_str());
            auto it = FuncTable.back().params_list.begin();
            for(;it != FuncTable.back().params_list.end();it++) {
                if(it->ident_kind == 0)
                    printf("para %s\n",it->name.c_str());
                else if(it->ident_kind == 1)
                    printf("para[] %s\n",it->name.c_str());
                else if(it->ident_kind == 2)
                    printf("para[][%d] %s\n",it->dimension_two,it->name.c_str());
            }
        }
        Block();    // this is the beginning of table // 10 20 22 51
        funcReturn();
        if(now_token - 2 != near_return_final && FuncTable.back().return_kind == 0)
            ErrorHanding_g(outLexicalLine[now_token - 1]);
    }
    printLexicalExtra("<FuncDef>\n");
    return true;
}
bool MainFunc() {
    printLexicalOutput(now_token++);  // int
    GlobalFuncTable(now_token,0);
    newFunc(outLexicalWord[now_token]);
    printLexicalOutput(now_token++);    // main
    printLexicalOutput(now_token++);    // (
    printLexicalOutput(now_token++); // )
    if(QuaternionOutPut) {
        printf("%s %s()\n",FuncTable.back().return_kind == 1?"void":"int",FuncTable.back().name.c_str());
    }
    if(!allGlobalGet.empty()) {
        auto it = allGlobalGet.begin();
        for(;it != allGlobalGet.end();it++)
            getInt(*it);
    }
    vector<identTable> symbolTable;
    allSymbolTable.emplace_back(symbolTable);
    Block();
    if(now_token - 2 != near_return_final)
        ErrorHanding_g(outLexicalLine[now_token - 1]);
    printLexicalExtra("<MainFuncDef>\n");
    return true;
}
void Syntactic() {
    while(now_token < outLexicalToken.size() && Decl());
    Global = false;
    while(now_token < outLexicalToken.size() && Func());
    InMain = true;
    if(now_token < outLexicalToken.size())
        MainFunc();
    printLexicalExtra("<CompUnit>\n");
}
#endif //COMPILER_SYNTACTIC_H