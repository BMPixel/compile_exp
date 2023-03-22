%{
#include <stdio.h>
#include <stdlib.h>
#include "syntax.tab.h"
/* #define YYDEBUG 1 */
int yylex();
void yyerror(const char *s);
void yyrestart(FILE *f);

// Node of the AST
typedef struct Node {
    char *name;
    int lineno;
    union {
        int type_int;
        float type_float;
        double type_double;
        char *str;
    } value;
    struct Node *first_child;
    struct Node *next_sibling;
} Node;
%}

%union {
    int type_int;
    float type_float;
    double type_double;
}

%token <type_int> INT
%token <type_float> FLOAT
%token ID SEMI COMMA ASSIGNOP RELOP PLUS MINUS STAR DIV AND OR DOT NOT TYPE LP RP LB RB LC RC STRUCT RETURN IF ELSE WHILE

%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left STAR DIV
%left NOT
%left DOT LB RB LP RP 


%%
Program: ExtDefList
    ;
ExtDefList: ExtDef ExtDefList
    | /* empty */
    ;
ExtDef: Specifier ExtDecList SEMI
    | Specifier SEMI
    | Specifier FunDec CompSt
    | error SEMI
    ;
ExtDecList: VarDec
    | VarDec COMMA ExtDecList
    ;
Specifier: TYPE
    | StructSpecifier
    ;
StructSpecifier: STRUCT OptTag LC DefList RC
    | STRUCT Tag
    ;
OptTag: ID
    | /* empty */
    ;
Tag: ID
    ;
VarDec: ID
    | VarDec LB INT RB
    ;
FunDec: ID LP VarList RP
    | ID LP RP
    | error RP
    ;
VarList: ParamDec COMMA VarList
    | ParamDec
    ;
ParamDec: Specifier VarDec
    ;
CompSt: LC DefList StmtList RC
    | error RC
    ;
StmtList: Stmt StmtList
    | /* empty */
    ;
Stmt: Exp SEMI
    | error SEMI
    | CompSt
    | RETURN Exp SEMI
    | IF LP Exp RP Stmt
    | IF LP Exp RP Stmt ELSE Stmt
    | WHILE LP Exp RP Stmt
    ;
DefList: Def DefList
    | /* empty */
    ;
Def: Specifier DecList SEMI
    | error SEMI
    ;
DecList: Dec COMMA DecList
    | Dec
    ;
Dec: VarDec
    | VarDec ASSIGNOP Exp
    ;
Exp: Exp ASSIGNOP Exp
    | Exp AND Exp
    | Exp OR Exp
    | Exp RELOP Exp
    | Exp PLUS Exp
    | Exp MINUS Exp
    | Exp STAR Exp
    | Exp DIV Exp
    | LP Exp RP
    | MINUS Exp
    | NOT Exp
    | ID LP Args RP
    | ID LP RP
    | Exp LB Exp RB
    | Exp DOT ID
    | ID
    | INT
    | FLOAT
    | error RP
    ;
Args: Exp COMMA Args
    | Exp
    ;
%%

int main(int argc, char **argv) {
    if (argc <= 1) {
        fprintf(stderr, "Usage: %s <file>", argv[0]);
        return 1;
    }
    FILE *f = fopen(argv[1], "r");
    if (!f) {
        fprintf(stderr, "Could not open file %s for reading", argv[1]);
        return 1;
    }
    yyrestart(f);
    /* yydebug = 1; */
    yyparse();
    return 0;
}
