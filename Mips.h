//
// Created by hhw on 2022/11/2.
//

#ifndef COMPILER_MIPS_H
#define COMPILER_MIPS_H
#include <utility>

#include "Dependencies.h"
#include "ErrorHanding.h"
FILE *fd = fopen("mips.txt","w++");
FILE *fh = fopen("tmp.txt","w++");
vector<identTable> allGlobalVar;

struct reg {
    string name = "NULL!";
    string reg;
    bool used = false;
} ErrorReg;
vector<reg> load;
vector<bool> opLoad;
vector<string> ParamsGet;
// zero at
reg retReg[2]; // v0 v1
reg paramReg[4]; // a0 a1 a2 a3

reg varReg[10]; // t6 - t7 s0 - s7
int gpReg;

void relaxReg(const string& name) {
    for(int i = 0;i <= 9;i++) {
        if(name == varReg[i].reg) {
            varReg[i].used = false;
            return;
        }
    }
    for(int i = 1;i <= 3;i++) {
        if(name == paramReg[i].reg) {
            paramReg[i].used = false;
            return;
        }
    }
}

void checkSt(const st& melt) {
    if(melt.melt) {
        for(int i = 0;i <= 9;i++) {
            if(melt.reg == varReg[i].reg) {
                varReg[i].used = false;
                return;
            }
        }
    }
}
void uploadToMem(const st& opTwo) {    // now we get the address, and push it to it
    fprintf(fh,"lw %s 0(%s)\n",opTwo.reg.c_str(),opTwo.reg.c_str());
}
void initReg() {
    retReg[0].reg = "$v0";
    retReg[1].reg = "$v1";
    paramReg[0].reg = "$a0";
    paramReg[1].reg = "$a1";
    paramReg[2].reg = "$a2";
    paramReg[3].reg = "$a3";
    // opReg
    varReg[0].reg = "$t6";
    varReg[1].reg = "$t7";
    varReg[2].reg = "$s0";
    varReg[3].reg = "$s1";
    varReg[4].reg = "$s2";
    varReg[5].reg = "$s3";
    varReg[6].reg = "$s4";
    varReg[7].reg = "$s5";
    varReg[8].reg = "$s6";
    varReg[9].reg = "$s7";
    gpReg = 0;
}
void printOpNumber(st op) {
    if(op.reg != "NULL!") {
        fprintf(fh,"%s ",op.reg.c_str());   // reg
        return;
    }
    if(op.kind == 0) {  // int
        fprintf(fh,"%d ",op.value);
        return;
    }
    if(*op.reg.begin() != '$' && op.fg_offset == -1) {  // global
        fprintf(fh,"%s ",op.name.c_str());
        return;
    }
    if(*op.reg.begin() != '$' && op.fg_offset != -1) {  // fp
        fprintf(fh,"%d($fp) ",op.fg_offset);
        return;
    }
}
reg getReg() {
    int i = 0;
    for(;i <= 9;i++) {
        if (!varReg[i].used) {
            allSymbolTable.back().back().reg = varReg[i].reg;
            varReg[i].name = allSymbolTable.back().back().name;
            varReg[i].used = true;
            return varReg[i];
        }
    }
    allSymbolTable.back().back().fg_offset = gpReg;
    gpReg += 4;
    return ErrorReg;
}
reg getArrayReg(int one,int two) {
    allSymbolTable.back().back().fg_offset = gpReg;
    if(two != 0)
        gpReg += one * two * 4;
    else
        gpReg += one * 4;
    return ErrorReg;
}
#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
void OpReg(const string& name,st& opOne,st& opTwo,const string& op) {
    if(opOne.isAddress) {
        uploadToMem(opOne);
        opOne.isAddress = false;
    }
    if(opTwo.isAddress) {
        uploadToMem(opTwo);
        opTwo.isAddress = false;
    }
    if((opOne.kind == 0 && op == "MULT") || (opOne.kind == 0 && op == "PLUS") || (opOne.kind == 0 && op == "BitAnd")) {
        OpReg(name,opTwo,opOne,op);
        return;
    }
    if(op == "MULT") {
        fprintf(fh,"mul %s ",name.c_str());
        if(opOne.reg != "NULL!")
            fprintf(fh,"%s ",opOne.reg.c_str());
        else
            fprintf(fh,"%d($fp) ",opOne.fg_offset);
        if(opTwo.kind == 0)
            fprintf(fh,"%d \n",opTwo.value);
        else {
            if(opTwo.reg != "NULL!")
                fprintf(fh,"%s\n",opTwo.reg.c_str());
            else
                fprintf(fh,"%d($fp)\n",opTwo.fg_offset);
        }
    }
    else if(op == "DIV") {
        st tmp;
        bool kind = false;
        if(opOne.kind == 0) {
            kind = true;
            tmp.reg = getCode();
            fprintf(fh,"li %s %d\n",tmp.reg.c_str(),opOne.value);
            ret(tmp.reg);
        }
        fprintf(fh,"div %s ",name.c_str());
        if(opOne.reg != "NULL!")
            fprintf(fh,"%s ",opOne.reg.c_str());
        else if(kind)
            fprintf(fh,"%s ",tmp.reg.c_str());
        else
            fprintf(fh,"%d($fp) ",opOne.fg_offset);
        if(opTwo.kind == 0)
            fprintf(fh,"%d \n",opTwo.value);
        else {
            if(opTwo.reg != "NULL!")
                fprintf(fh,"%s\n",opTwo.reg.c_str());
            else
                fprintf(fh,"%d($fp)\n",opTwo.fg_offset);
        }
    }
    else if(op == "PLUS") {
        fprintf(fh,"add %s ",name.c_str());
        if(opOne.kind == 0)
            fprintf(fh,"%d ",opOne.value);
        else {
            if(opOne.reg != "NULL!")
                fprintf(fh,"%s ",opOne.reg.c_str());
            else
                fprintf(fh,"%d($fp) ",opOne.fg_offset);
        }
        if(opTwo.kind == 0)
            fprintf(fh,"%d \n",opTwo.value);
        else {
            if(opTwo.reg != "NULL!")
                fprintf(fh,"%s\n",opTwo.reg.c_str());
            else
                fprintf(fh,"%d($fp)\n",opTwo.fg_offset);
        }
    }
    else if(op == "MINU") {
        bool kind = false;
        st tmp;
        if(opOne.kind == 0) {
            tmp.reg = getCode();
            fprintf(fh,"li %s %d\n",tmp.reg.c_str(),opOne.value);
            ret(tmp.reg);
            kind = true;
        }
        fprintf(fh,"sub %s ",name.c_str());
        if(opOne.reg != "NULL!")
            fprintf(fh,"%s ",opOne.reg.c_str());
        else if(kind)
            fprintf(fh,"%s ",tmp.reg.c_str());
        else
            fprintf(fh,"%d($fp) ",opOne.fg_offset);
        if(opTwo.kind == 0)
            fprintf(fh,"%d \n",opTwo.value);
        else {
            if(opTwo.reg != "NULL!")
                fprintf(fh,"%s\n",opTwo.reg.c_str());
            else
                fprintf(fh,"%d($fp)\n",opTwo.fg_offset);
        }
    }
    else if(op == "MOD") {
        st tmp;
        bool kind = false;
        if(opOne.kind == 0) {
            kind = true;
            tmp.reg = getCode();
            fprintf(fh,"li %s %d\n",tmp.reg.c_str(),opOne.value);
            ret(tmp.reg);
        }
        fprintf(fh,"div %s ",name.c_str());
        if(opOne.reg != "NULL!")
            fprintf(fh,"%s ",opOne.reg.c_str());
        else if(kind)
            fprintf(fh,"%s ",tmp.reg.c_str());
        else
            fprintf(fh,"%d($fp) ",opOne.fg_offset);
        if(opTwo.kind == 0)
            fprintf(fh,"%d \n",opTwo.value);
        else {
            if(opTwo.reg != "NULL!")
                fprintf(fh,"%s\n",opTwo.reg.c_str());
            else
                fprintf(fh,"%d($fp)\n",opTwo.fg_offset);
        }
        fprintf(fh,"mfhi %s \n",name.c_str());
    }
    else if(op == "BitAnd") {
        if(opTwo.kind != 0)
            fprintf(fh,"and %s ",name.c_str());
        else
            fprintf(fh,"andi %s ",name.c_str());
        if(opOne.kind == 0)
            fprintf(fh,"%d ",opOne.value);
        else {
            if(opOne.reg != "NULL!")
                fprintf(fh,"%s ",opOne.reg.c_str());
            else
                fprintf(fh,"%d($fp) ",opOne.fg_offset);
        }
        if(opTwo.kind == 0)
            fprintf(fh,"%d \n",opTwo.value);
        else {
            if(opTwo.reg != "NULL!")
                fprintf(fh,"%s\n",opTwo.reg.c_str());
            else
                fprintf(fh,"%d($fp)\n",opTwo.fg_offset);
        }
    }
    checkSt(opOne);
    checkSt(opTwo);
}
void compareReg(const string& if_ok,const string& if_no,const st& one,const st& two,const string& op) {
    if(op == "LSS") {   // <
        if(one.kind == 0)
            fprintf(fh,"sgt ");
        else
            if(two.kind == 0)
                fprintf(fh,"slti ");
            else
                fprintf(fh,"slt ");
    }
    else if(op == "LEQ") {  // <=
        if(one.kind == 0)
            fprintf(fh,"sgt ");
        else
            fprintf(fh,"sle ");
    }
    else if(op == "GRE") {  // >
        if(one.kind == 0)
            fprintf(fh,"sle ");
        else
            fprintf(fh,"sgt ");
    }
    else if(op == "GEQ")  {  // >=
        if(one.kind == 0)
            fprintf(fh,"slti ");
        else
            fprintf(fh,"sge ");
    }
    else if(op == "EQL") {  // ==
        fprintf(fh,"seq ");
    }
    else if(op == "NEQ") {  // !=
        fprintf(fh,"sne ");
    }
    st opTmp;
    opTmp.kind = 1;
    opTmp.reg = getCode();
    opTmp.name = opTmp.reg;
    allStack.emplace(opTmp);
    fprintf(fh,"%s ",opTmp.reg.c_str());
    if(one.kind == 0) {
        fprintf(fh,"%s ",two.reg.c_str());
        fprintf(fh,"%d \n",one.value);
    }
    else {
        fprintf(fh,"%s ",one.reg.c_str());
        if(two.kind == 0)
            fprintf(fh,"%d \n",two.value);
        else
            fprintf(fh,"%s \n",two.reg.c_str());
    }
}
#pragma clang diagnostic pop
int mipsInit() {
    int i = 1;
    auto it = data_string.begin();
    fprintf(fd,".data\n");
    for(;it != data_string.end();it++,i++)
        fprintf(fd,"str_%d: .asciiz \"%s\"\n",i,(*it).c_str());
    auto el = allGlobalVar.begin();
    for(;el != allGlobalVar.end();el++) {
        if(el->ident_kind == 0)
            fprintf(fd,"%s: .word %d\n",(*el).name.c_str(),(*el).value);
        else {
            fprintf(fd,"%s: .word ",(*el).name.c_str());
            auto pk = el->arrayValue.begin();
            for(;pk != el->arrayValue.end();pk++)
                fprintf(fd,"%d ",*pk);
            fprintf(fd,"\n");
        }
    }
    fprintf(fd,".text\n"
               "li $fp, 0x10040000\n"
               "j main\n"
               "nop\n");
    int ch;
    fclose(fh);
    fh = fopen("tmp.txt","r");
    while ((ch=fgetc(fh))!=EOF) {
        fputc(ch,fd);
    }
    fclose(fd);
    fclose(fh);
    return 0;
}
void equalOp(st &opOne, st opTwo) {
    bool get = false;
    if(opTwo.isAddress) {
        uploadToMem(opTwo);
        opTwo.isAddress = false;
    }
    if(opTwo.kind != 0 && *opTwo.reg.begin() != '$' && opTwo.fg_offset == -1) {
        opTwo.reg = getCode();
        fprintf(fh,"lw %s %s\n",opTwo.reg.c_str(),opTwo.name.c_str());
        get = true;
    }
    else if(opTwo.kind != 0 && *opTwo.reg.begin() != '$' && opTwo.fg_offset != -1) {
        opTwo.reg = getCode();
        fprintf(fh,"lw %s %d($fp)\n",opTwo.reg.c_str(),opTwo.fg_offset);
        get = true;
    }
    else if(opTwo.kind == 0) {
        opTwo.reg = getCode();
        fprintf(fh,"li %s %d\n",opTwo.reg.c_str(),opTwo.value);
        get = true;
    }
    if(opOne.isAddress) {
        opOne.isAddress = false;
        fprintf(fh,"sw %s 0(%s)\n",opTwo.reg.c_str(),opOne.reg.c_str());
        if(get)
            ret(opTwo.reg);
        return ;
    }
    if(opTwo.kind == 0) {
        if(opOne.reg != "NULL!")
            fprintf(fh,"li %s %d\n",opOne.reg.c_str(),opTwo.value);
        else {
            fprintf(fh,"sw ");
            printOpNumber(opTwo);
            printOpNumber(opOne);
            fprintf(fh,"\n");
        }
        if(get)
            ret(opTwo.reg);
        checkSt(opTwo);
        return;
    }
    if(opOne.reg == "NULL!") {
        fprintf(fh,"sw ");
        printOpNumber(opTwo);
        printOpNumber(opOne);
        fprintf(fh,"\n");
    }
    else {
        fprintf(fh,"or ");
        printOpNumber(opOne);
        printOpNumber(opTwo);
        fprintf(fh,"$zero\n");
    }
    if(get)
        ret(opTwo.reg);
    checkSt(opTwo);
    ret(opOne.reg);
}
void newGlobalVar(const identTable& a) {
    allGlobalVar.emplace_back(a);
}
void printVarDef(const identTable& a,int value,int off) {
    if(a.ident_kind == 0)
        printf("%s = %d\n",a.name.c_str(),value);
    else
        printf("%s[%d] = %d\n",a.name.c_str(),off,value);
    if(a.reg != "NULL!")
        fprintf(fh,"li %s %d\n",a.reg.c_str(),value);
    else {
        st b;
        b.kind = 0;
        b.value = value;
        st tmp;
        tmp.kind = 1;
        tmp.fg_offset = a.fg_offset + off * 4;
        equalOp(tmp,b);
    }
}
void printStr(int str) {
    fprintf(fh,"li $v0 4\n");
    fprintf(fh,"la $a0 str_%d\n",str);
    fprintf(fh,"syscall\n");
}
void printVar(st &a) {
    if(a.isAddress) {
        a.isAddress = false;
        uploadToMem(a);
    }
    fprintf(fh,"li $v0 1\n");
    if(a.reg != "NULL!") {
        fprintf(fh,"or $a0 %s $zero\n",a.reg.c_str());
    }
    else if(a.fg_offset == -1) {
        fprintf(fh,"lw $a0 %s\n",a.name.c_str());
    }
    else {
        fprintf(fh,"lw $a0 %d($fp)\n",a.fg_offset);
    }
    fprintf(fh,"syscall\n");
    checkSt(a);
}
void printNum(int number) {
    fprintf(fh,"li $v0 1\n");
    fprintf(fh,"li $a0 %d\n",number);
    fprintf(fh,"syscall\n");
}
void newFunc(const string& FuncName) {
    int i;
    retReg[0].used = false;
    retReg[1].used = false;
    gpReg = 0;
    for(i = 0;i <= 9;i++)
        varReg[i].used = false;
    for(i = 0;i <= 4;i++)
        code[i] = true;
    fprintf(fh,"\n%s:\n",FuncName.c_str());
}
void funcReturn() {
    fprintf(fh,"jr $ra\n");
}
void mainReturn() {
    fprintf(fh,"li $v0 10\n");
    fprintf(fh,"syscall\n");
}
void setReturn(st &a) {
    if(a.isAddress) {
        uploadToMem(a);
        a.isAddress = false;
    }
    if(a.offset == -1) {
        if(a.kind == 0) {
            fprintf(fh,"li $v0 %d\n",a.value);
            return;
        }
        if(a.reg != "NULL!") {
            fprintf(fh,"move $v0 %s\n",a.reg.c_str());
            return;
        }
        if(a.fg_offset != -1) {
            fprintf(fh,"lw $v0 %d($fp)\n",a.fg_offset);
            return;
        }
        fprintf(fh,"lw $v0 %s\n",a.name.c_str());
    }
    checkSt(a);
}
void getInt(st& a) {
    fprintf(fh,"li $v0 5\nsyscall\n");
    st b;
    b.kind = 1;
    b.reg = "$v0";
    b.name = "$v0";
    equalOp(a,b);
}
st negReg(st& a) {
    if(a.isAddress) {
        uploadToMem(a);
        a.isAddress = false;
    }
    if(a.reg != "NULL!") {
        ret(a.reg);
        string tmp = a.reg;
        a.reg = getCode();
        fprintf(fh,"neg %s %s\n",a.reg.c_str(),tmp.c_str());
        return a;
    }
    a.reg = getCode();
    if(a.fg_offset != -1) {
        fprintf(fh,"lw %s %d($fp)\n",a.reg.c_str(),a.fg_offset);
    }
    else
        fprintf(fh,"lw %s %s\n",a.reg.c_str(),a.name.c_str());
    fprintf(fh,"neg %s %s\n",a.reg.c_str(),a.reg.c_str());
    return a;
}
st notReg(st& a) {
    if(a.isAddress) {
        uploadToMem(a);
        a.isAddress = false;
    }
    if(a.reg != "NULL!") {
        ret(a.reg);
        string tmp = a.reg;
        a.reg = getCode();
        fprintf(fh,"seq %s %s 0\n",a.reg.c_str(),tmp.c_str());
        return a;
    }
    a.reg = getCode();
    if(a.fg_offset != -1) {
        fprintf(fh,"lw %s %d($fp)\n",a.reg.c_str(),a.fg_offset);
    }
    else
        fprintf(fh,"lw %s %s\n",a.reg.c_str(),a.name.c_str());
    fprintf(fh,"seq %s %s 0\n",a.reg.c_str(),a.reg.c_str());
    return a;
}
st yesReg(st& a) {
    if(a.isAddress) {
        uploadToMem(a);
        a.isAddress = false;
    }
    if(a.reg != "NULL!") {
        ret(a.reg);
        string tmp = a.reg;
        a.reg = getCode();
        fprintf(fh,"sne %s %s 0\n",a.reg.c_str(),tmp.c_str());
        return a;
    }
    a.reg = getCode();
    if(a.fg_offset != -1) {
        fprintf(fh,"lw %s %d($fp)\n",a.reg.c_str(),a.fg_offset);
    }
    else
        fprintf(fh,"lw %s %s\n",a.reg.c_str(),a.name.c_str());
    fprintf(fh,"sne %s %s 0\n",a.reg.c_str(),a.reg.c_str());
    return a;
}
int saveReg() {
    int sw = 0;
    int psw = 0;
    int i;
    load.push_back(retReg[0]);
    load.push_back(retReg[1]);
    for(i = 0;i <= 3;i++)
        load.push_back(paramReg[i]);
    for(i = 0;i <= 9;i++)
        load.push_back(varReg[i]);
    for(i = 0;i < 5;i++)
        opLoad.push_back(code[i]);
    auto it = load.begin();
    for(;it != load.end();it++) {
        if(it->used)
            sw += 4;    // all reg
    }
    auto el = opLoad.begin();
    for(;el != opLoad.end();el++) {
        if(!*el)
            sw += 4;    // t0 - t4
    }
    sw += 4;        // ra
    fprintf(fh,"addi $sp $sp %d\n",-sw);
    it = load.begin();
    for(;it != load.end();it++) {
        if(it->used) {
            fprintf(fh,"sw %s %d($sp)\n",it->reg.c_str(),psw);
            psw += 4;
        }
    }
    el = opLoad.begin();
    for(;el != opLoad.end();el++) {
        if(!*el) {
            fprintf(fh,"sw $t%td %d($sp)\n",el - opLoad.begin(),psw);
            psw += 4;
        }
    }
    fprintf(fh,"sw $ra %d($sp)\n",psw);
    return sw;
}
void loadReg(int sw) {
    int i;
    int psw = 0;
    auto it = load.begin();
    retReg[0] = *it;    it++;
    retReg[1] = *it;    it++;
    for(i = 0;i <= 3;i++,it++) {
        paramReg[i] = *it;
    }
    for(i = 0;i <= 9;i++,it++) {
        varReg[i] = *it;
    }
    auto el = opLoad.begin();
    for(i = 0;i <= 4;i++,el++) {
        code[i] = *el;
    }
    it = load.begin();
    for(;it != load.end();it++) {
        if(it->used) {
            fprintf(fh,"lw %s %d($sp)\n",it->reg.c_str(),psw);
            psw += 4;
        }
    }
    el = opLoad.begin();
    for(;el != opLoad.end();el++) {
        if(!*el) {
            fprintf(fh,"lw $t%td %d($sp)\n",el - opLoad.begin(),psw);
            psw += 4;
        }
    }
    fprintf(fh,"lw $ra %d($sp)\n",psw);
    fprintf(fh,"addi $sp $sp %d\n",sw);
    load.clear();
    opLoad.clear();
}
void moveGp(int now_fp,int para) {
    vector<st> all;
    int func_fp = 0;
    int i = 0;
    for(;i < para;i++) {
        if(allStack.top().isAddress) {
            uploadToMem(allStack.top());
            allStack.top().isAddress = false;
        }

        all.push_back(allStack.top());
        allStack.pop();
    }
    auto el = all.crbegin();
    for(;el != all.crend();el++) {
        if(el->kind == 0) {
            fprintf(fh,"li $a0 %d\n",el->value);
            fprintf(fh,"sw $a0 %d($fp)\n",now_fp + func_fp);
            func_fp+=4;
            continue;
        }
        if(el->reg != "NULL!") {
            fprintf(fh,"sw %s %d($fp)\n",el->reg.c_str(),now_fp + func_fp);
            func_fp+=4;
            continue;
        }
        if(el->reg == "NULL!") {
            if(el->fg_offset == -1) {
                fprintf(fh,"lw $a0 %s\n",el->name.c_str());
                fprintf(fh,"sw $a0 %d($fp)\n",now_fp + func_fp);
                func_fp+=4;
                continue;
            }
            if(el->fg_offset != -1) {
                fprintf(fh,"lw $a0 %d($fp)\n",el->fg_offset);
                fprintf(fh,"sw $a0 %d($fp)\n",now_fp + func_fp);
                func_fp+=4;
                continue;
            }
        }
    }
    if(!ParamsGet.empty()) {
        auto pk = ParamsGet.begin();
        for(;pk != ParamsGet.end();pk++) {
            relaxReg(*pk);
        }
        ParamsGet.clear();
    }
}
st GetArrayHead(const identTable& a) {
    st tmp;
    tmp.name = getCode();
    tmp.reg = tmp.name;
    tmp.kind = 1;
    tmp.offset = -1;
    tmp.fg_offset = -1;
    if(a.isGlobal)
        fprintf(fh,"la %s %s\n",tmp.reg.c_str(),a.name.c_str());
    else if(inParamsList(a))
        fprintf(fh,"lw %s %d($fp)\n",tmp.reg.c_str(),a.fg_offset);
    else
        fprintf(fh,"addi %s $fp %d\n",tmp.reg.c_str(),a.fg_offset);
    return tmp;
}

#endif //COMPILER_MIPS_H
