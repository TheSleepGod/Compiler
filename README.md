### 抄来的文法定义
CompUnit		→ {Decl} {FuncDef} MainFuncDef
Decl			→ ConstDecl | VarDecl
ConstDecl		→ 'const' BType ConstDef { ',' ConstDef } ';'
BType			→ 'int'
ConstDef		→ Ident { '[' ConstExp ']' } '=' ConstInitVal
ConstInitVal		→ ConstExp | '{' [ ConstInitVal { ',' ConstInitVal } ] '}'
VarDecl			→ BType VarDef { ',' VarDef } ';' 
VarDef			→ Ident { '[' ConstExp ']' } | Ident { '[' ConstExp ']' } '=' InitVal
InitVal			→ Exp | '{' [ InitVal { ',' InitVal } ] '}'
FuncDef			→ FuncType Ident '(' [FuncFParams] ')' Block
MainFuncDef		→ 'int' 'main' '(' ')' Block
FuncType		→ 'void' | 'int'
FuncFParams		→ FuncFParam { ',' FuncFParam }
FuncFParam		→ BType Ident ['[' ']' { '[' ConstExp ']' }]
Block			→ '{' { BlockItem } '}'
BlockItem		→ Decl | Stmt
Stmt			→ LVal '=' Exp ';'
			| Block
			| 'if' '(' Cond ')' Stmt [ 'else' Stmt ]
			| 'break' ';' 
			| 'continue' ';'
			| 'return' [Exp] ';' 
			| LVal '=' 'getint''('')'';'
			| 'printf''('FormatString{','Exp}')'';'
Exp			→ AddExp
Cond			→ LOrExp 
LVal			→ Ident {'[' Exp ']'}
PrimaryExp		→ '(' Exp ')' | LVal | Number
Number			→ IntConst
UnaryExp		→ PrimaryExp | Ident '(' [FuncRParams] ')' | UnaryOp UnaryExp
UnaryOp			→ '+' | '−' | '!'
FuncRParams		→ Exp { ',' Exp }
MulExp			→ UnaryExp | MulExp ('*' | '/' | '%') UnaryExp
AddExp			→ MulExp | AddExp ('+' | '−') MulExp
RelExp			→ AddExp | RelExp ('<' | '>' | '<=' | '>=') AddExp
EqExp			→ RelExp | EqExp ('==' | '!=') RelExp
LAndExp			→ EqExp | LAndExp '&&' EqExp
LOrExp			→ LAndExp | LOrExp '||' LAndExp
ConstExp		→ AddExp

### 好像是这样用的运行方式
c++项目，纯递归下降法，带少量优化，详见PDF

新建一个项目，复制.h和.cpp文件，并创建full文件夹。

在full中新建 testfile.txt 文件，将要编译的源代码写到该文件中。

程序入口为main.cpp，其中有调试选项，开启可输出中间结果。

### 还能记得的主要问题

编译为mips文件时选用的指令为16位，运算容易溢出，可在mips.h修改

无循环优化

递归调用栈复制

数组相对位置计算未优化
