//
// Created by hhw on 2022/10/27.
//
#include "Dependencies.h"
#include "Mips.h"
#ifndef COMPILER_QUATERNION_H
#define COMPILER_QUATERNION_H



void equalOperation(st& opOne,const st& opTwo) {
    printf("%s = %s\n",opOne.name.c_str(),opTwo.name.c_str());
    equalOp(opOne,opTwo);
    ret(opTwo.reg);
}

void operation(const string& op) {
    if(allStack.size() >= 2) {
        st opTwo = allStack.top();
        allStack.pop();
        st opOne = allStack.top();
        allStack.pop();
        if(opOne.kind == 0 && opTwo.kind == 0) {
            if(op == "MULT")
                opOne.value = opOne.value * opTwo.value;
            else if(op == "DIV")
                opOne.value = opOne.value / opTwo.value;
            else if(op == "PLUS")
                opOne.value = opOne.value + opTwo.value;
            else if(op == "MINU")
                opOne.value = opOne.value - opTwo.value;
            else if(op == "MOD")
                opOne.value = opOne.value % opTwo.value;
            else if(op == "BitAnd")
                opOne.value = opOne.value & opTwo.value;
            allStack.push(opOne);
        }
        else {
            if(opOne.kind != 0 && *opOne.reg.begin() != '$' && opOne.fg_offset == -1) {
                opOne.reg = getCode();
                fprintf(fh,"lw %s %s\n",opOne.reg.c_str(),opOne.name.c_str());
            }
            else if(opOne.kind != 0 && *opOne.reg.begin() != '$' && opOne.fg_offset != -1) {
                opOne.reg = getCode();
                fprintf(fh,"lw %s %d($fp)\n",opOne.reg.c_str(),opOne.fg_offset);
            }
            if(opTwo.kind != 0 && *opTwo.reg.begin() != '$' && opTwo.fg_offset == -1) {
                opTwo.reg = getCode();
                fprintf(fh,"lw %s %s\n",opTwo.reg.c_str(),opTwo.name.c_str());
            }
            else if(opTwo.kind != 0 && *opTwo.reg.begin() != '$' && opTwo.fg_offset != -1) {
                opTwo.reg = getCode();
                fprintf(fh,"lw %s %d($fp)\n",opTwo.reg.c_str(),opTwo.fg_offset);
            }
            ret(opOne.name);
            ret(opTwo.name);
            ret(opOne.reg);
            ret(opTwo.reg);
            string name = getCode();
            OpReg(name,opOne,opTwo,op);
            printf("%s = ",name.c_str());
            if(opOne.kind == 0)
                printf("%d ",opOne.value);
            else if(opOne.kind == 1 && opOne.offset != -1)
                printf("%s(%d) ",opOne.name.c_str(),opOne.offset);
            else if(opOne.kind == 1)
                printf("%s ",opOne.name.c_str());
            if(op == "MULT")
                printf("*");
            else if(op == "DIV")
                printf("/");
            else if(op == "PLUS")
                printf("+");
            else if(op == "MINU")
                printf("-");
            else if(op == "MOD")
                printf("%%");
            if(opTwo.kind == 0)
                printf(" %d ",opTwo.value);
            else if(opTwo.kind == 1 && opTwo.offset != -1)
                printf(" %s(%d)",opTwo.name.c_str(),opTwo.offset);
            else if(opTwo.kind == 1)
                printf(" %s ",opTwo.name.c_str());
            printf("\n");
            opOne.kind = 1;
            opOne.name = name;
            opOne.reg = opOne.name;
            allStack.push(opOne);
        }
    }
}

int StringToNumber(string Number) {
    int ans = 0;
    auto it = Number.begin();
    for(;it!=Number.end();it++) {
        ans = ans * 10 + *it - '0';
    }
    return ans;
}
void printDef(const identTable& a) {
    if(!a.changeable)
        printf("const ");
    if(a.ident_kind == 0) {
        if(a.init)
            printf("int %s = %d\n",a.name.c_str(),a.value);
        else
            printf("int %s \n",a.name.c_str());
    }
    else {
        printf("int %s",a.name.c_str());
        printf("[%d]",a.dimension_one);
        if(a.ident_kind >= 2)
            printf("[%d]",a.dimension_two);
        printf("\n");
    }
}
st GetArray(const identTable& a) {
    if(a.isGlobal)
        printf("Global a[]\n");
    else
        printf("Var a[]\n");
    return GetArrayHead(a);
}

void ParamsArray(const identTable& a,st tmp[],int return_value) {   // push the address of array in Stack
    printf("push %s\n",a.name.c_str());
    if(a.ident_kind == 1 || return_value == 2) {
        allStack.push(GetArrayHead(a));
        return;
    }
    if(a.ident_kind == 2 && return_value == 1) {
        if(tmp[1].kind == 0) {
            tmp[1].value = tmp[1].value * a.dimension_two * 4;
            allStack.push(tmp[1]);
        }
        else {
            tmp[0].kind = 0;
            tmp[0].value = a.dimension_two * 4;
            allStack.push(tmp[1]);
            allStack.push(tmp[0]);
            operation("MULT");
        }
        allStack.push(GetArrayHead(a));
        operation("PLUS");
    }
}
void compare(const string& op,const string& if_ok,const string& if_no) {    // op is > = < -->is basic  && || is top
    if(allStack.size() >= 2) {
        st opTwo = allStack.top();
        allStack.pop();
        st opOne = allStack.top();
        allStack.pop();
        if(opOne.isAddress) {
            uploadToMem(opOne);
            opOne.isAddress = false;
        }
        if(opTwo.isAddress) {
            uploadToMem(opTwo);
            opTwo.isAddress = false;
        }
        if(opOne.kind == 0 && opTwo.kind == 0) {
            if(op == "LSS") {   // <
                if(opOne.value < opTwo.value) {
                    opOne.value = 1;
                    allStack.emplace(opOne);
                }
                else {
                    opOne.value = 0;
                    allStack.emplace(opOne);
                }
            }
            else if(op == "LEQ") {  // <=
                if(opOne.value <= opTwo.value) {
                    opOne.value = 1;
                    allStack.emplace(opOne);
                }
                else {
                    opOne.value = 0;
                    allStack.emplace(opOne);
                }
            }
            else if(op == "GRE") {  // >
                if(opOne.value > opTwo.value) {
                    opOne.value = 1;
                    allStack.emplace(opOne);
                }
                else {
                    opOne.value = 0;
                    allStack.emplace(opOne);
                }
            }
            else if(op == "GEQ")  {  // >=
                if(opOne.value >= opTwo.value) {
                    opOne.value = 1;
                    allStack.emplace(opOne);
                }
                else {
                    opOne.value = 0;
                    allStack.emplace(opOne);
                }
            }
            else if(op == "EQL") {  // ==
                if(opOne.value == opTwo.value) {
                    opOne.value = 1;
                    allStack.emplace(opOne);
                }
                else {
                    opOne.value = 0;
                    allStack.emplace(opOne);
                }
            }
            else if(op == "NEQ") {  // !=
                if(opOne.value != opTwo.value) {
                    opOne.value = 1;
                    allStack.emplace(opOne);
                }
                else {
                    opOne.value = 0;
                    allStack.emplace(opOne);
                }
            }
        }
        else {
            if(opOne.kind != 0 && *opOne.reg.begin() != '$' && opOne.fg_offset == -1) {
                opOne.reg = getCode();
                fprintf(fh,"lw %s %s\n",opOne.reg.c_str(),opOne.name.c_str());
            }
            else if(opOne.kind != 0 && *opOne.reg.begin() != '$' && opOne.fg_offset != -1) {
                opOne.reg = getCode();
                fprintf(fh,"lw %s %d($fp)\n",opOne.reg.c_str(),opOne.fg_offset);
            }
            if(opTwo.kind != 0 && *opTwo.reg.begin() != '$' && opTwo.fg_offset == -1) {
                opTwo.reg = getCode();
                fprintf(fh,"lw %s %s\n",opTwo.reg.c_str(),opTwo.name.c_str());
            }
            else if(opTwo.kind != 0 && *opTwo.reg.begin() != '$' && opTwo.fg_offset != -1) {
                opTwo.reg = getCode();
                fprintf(fh,"lw %s %d($fp)\n",opTwo.reg.c_str(),opTwo.fg_offset);
            }
            ret(opOne.name);
            ret(opTwo.name);
            ret(opOne.reg);
            ret(opTwo.reg);
            compareReg(if_ok,if_no,opOne,opTwo,op);
            printQuaternion("compare ");
            if(opOne.kind == 0)
                printf("%d ",opOne.value);
            else
                printf("%s ",opOne.reg.c_str());
            if(opTwo.kind == 0)
                printf("%d ",opTwo.value);
            else
                printf("%s ",opTwo.reg.c_str());
            printf("%s\n",if_ok.c_str());
            printf("j %s\n",if_no.c_str());
        }
    }
    
}
#endif //COMPILER_QUATERNION_H
