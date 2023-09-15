#include <iostream>
#include "Lexical.h"
#include "Syntactic.h"
#include "Mips.h"
#include "debug.h"

using namespace std;
int main() {
        initReg();
//        string filename = "./full/testfile/testfile" + to_string(i) + ".txt";
//        string output = "./full/outfile/testfile" + to_string(i) + "-20373190-hhw-withoutChange-mips";
//        string output2 = "./full/outfile/testfile" + to_string(i) + "-20373190-hhw-withoutChange-quaternion";
//        fd = fopen(output.c_str(),"w++");
        freopen("testfile.txt","r",stdin);
        freopen("error.txt","w",stdout);
        Lexical();
        Syntactic();
        mipsInit();


    return 0;
}
