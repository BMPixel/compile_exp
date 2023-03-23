%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "ast.h"
#include "syntax.tab.h"
#define cnode(n, l, c, ...) create_node(n, l->lineno, c, (Node *[]){__VA_ARGS__})
/* #define YYDEBUG 1 */
int yylex();
void yyerror(const char *s);
void yyrestart(FILE *f);


Node* create_node(char *name, int lineno, int num_children, Node **children) {
    Node *node = (Node *)malloc(sizeof(Node));
    node->name = name;
    node->lineno = lineno;
    node->first_child = NULL;
    node->next_sibling = NULL;
    node->value_string = NULL;
    node->is_terminal = 0;

    Node *prev_child = NULL;
    for (int i = 0; i < num_children; ++i) {
        Node *child = children[i];
        if (!child) {
            continue;
        }
        if (prev_child) {
            prev_child->next_sibling = child;
        } else {
            node->first_child = child;
        }
        prev_child = child;
    }
    return node;
}

Node* create_terminal_node(char * name, int lineno, char* value) {
    Node *node = (Node *)malloc(sizeof(Node));
    node->name = name;
    node->lineno = lineno;
    node->first_child = NULL;
    node->next_sibling = NULL;
    node->is_terminal = 1;
    if (value) {
        node->value_string = (char *)malloc(sizeof(char) * (strlen(value) + 1));
        strcpy(node->value_string, value);
    } else {
        node->value_string = NULL;
    }
    return node;
}

Node* root;

void print_tree(Node *node, int level) {
    if (!node) return;
    for (int i = 0; i < level; ++i) {
        printf("  ");
    }
    if (node->is_terminal)
        if (node->value_string)
            printf("%s: %s\n", node->name, node->value_string);
        else
            printf("%s\n", node->name);
    else
        printf("%s (%d)\n", node->name, node->lineno);
    print_tree(node->first_child, level + 1);
    print_tree(node->next_sibling, level);
}

%}

%union {
    int type_int;
    float type_float;
    double type_double;
    Node* node_ptr;
}

%token <node_ptr> INT FLOAT ID SEMI COMMA ASSIGNOP RELOP PLUS MINUS STAR DIV AND OR DOT NOT TYPE LP RP LB RB LC RC STRUCT RETURN IF ELSE WHILE
%type <node_ptr> Program ExtDefList ExtDef ExtDecList Specifier StructSpecifier OptTag Tag VarDec FunDec VarList ParamDec CompSt StmtList Stmt DefList Def DecList Dec Exp Args

%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left STAR DIV
%left NOT
%left DOT LB RB LP RP 


%%
Program: ExtDefList { $$=cnode("Program", $1, 1, $1); root=$$; }
    ;
ExtDefList: ExtDef ExtDefList { $$=cnode("ExtDefList", $1, 2, $1, $2); }
    | /* empty */ { $$ = NULL; }
    ;
ExtDef: Specifier ExtDecList SEMI { $$=cnode("ExtDef", $1, 3, $1, $2, $3); }
    | Specifier SEMI { $$=cnode("ExtDef", $1, 2, $1, $2); }
    | Specifier FunDec CompSt { $$=cnode("ExtDef", $1, 3, $1, $2, $3); }
    | error SEMI { $$ = NULL; }
    ;
ExtDecList: VarDec { $$=cnode("ExtDecList", $1, 1, $1); }
    | VarDec COMMA ExtDecList { $$=cnode("ExtDecList", $1, 3, $1, $2, $3); }
    ;
Specifier: TYPE { $$=cnode("Specifier", $1, 1, $1); }
    | StructSpecifier { $$=cnode("Specifier", $1, 1, $1); }
    ;
StructSpecifier: STRUCT OptTag LC DefList RC { $$=cnode("StructSpecifier", $1, 5, $1, $2, $3, $4, $5); }
    | STRUCT Tag { $$=cnode("StructSpecifier", $1, 2, $1, $2); }
    ;
OptTag: ID { $$=cnode("OptTag", $1, 1, $1); }
    | /* empty */ { $$ = NULL; }
    ;
Tag: ID { $$=cnode("Tag", $1, 1, $1); }
    ;
VarDec: ID { $$=cnode("VarDec", $1, 1, $1); }
    | VarDec LB INT RB { $$=cnode("VarDec", $1, 3, $1, $2, $4); }
    ;
FunDec: ID LP VarList RP { $$=cnode("FunDec", $1, 4, $1, $2, $3, $4); }
    | ID LP RP { $$=cnode("FunDec", $1, 3, $1, $2, $3); }
    | error RP { $$ = NULL; }
    ;
VarList: ParamDec COMMA VarList { $$=cnode("VarList", $1, 3, $1, $2, $3); }
    | ParamDec { $$=cnode("VarList", $1, 1, $1); }
    ;
ParamDec: Specifier VarDec { $$=cnode("ParamDec", $1, 2, $1, $2); }
    ;
CompSt: LC DefList StmtList RC { $$=cnode("CompSt", $1, 4, $1, $2, $3, $4); }
    | error RC { $$ = NULL; }
    ;
StmtList: Stmt StmtList { $$=cnode("StmtList", $1, 2, $1, $2); }
    | /* empty */ { $$ = NULL; }
    ;
Stmt: Exp SEMI { $$=cnode("Stmt", $1, 2, $1, $2); }
    | error SEMI { $$ = NULL; }
    | CompSt { $$=cnode("Stmt", $1, 1, $1); }
    | RETURN Exp SEMI { $$=cnode("Stmt", $1, 3, $1, $2, $3); }
    | IF LP Exp RP Stmt { $$=cnode("Stmt", $1, 5, $1, $2, $3, $4, $5); }
    | IF LP Exp RP Stmt ELSE Stmt { $$=cnode("Stmt", $1, 7, $1, $2, $3, $4, $5, $6, $7); }
    | WHILE LP Exp RP Stmt { $$=cnode("Stmt", $1, 5, $1, $2, $3, $4, $5); }
    ;
DefList: Def DefList { $$=cnode("DefList", $1, 2, $1, $2); }
    | /* empty */ { $$ = NULL; }
    ;
Def: Specifier DecList SEMI { $$=cnode("Def", $1, 3, $1, $2, $3); }
    | error SEMI { $$ = NULL; }
    ;
DecList: Dec COMMA DecList { $$=cnode("DecList", $1, 3, $1, $2, $3); }
    | Dec { $$=cnode("DecList", $1, 1, $1); }
    ;
Dec: VarDec { $$=cnode("Dec", $1, 1, $1); }
    | VarDec ASSIGNOP Exp { $$=cnode("Dec", $1, 3, $1, $2, $3); }
    ;
Exp: Exp ASSIGNOP Exp { $$=cnode("Exp", $1, 3, $1, $2, $3); }
    | Exp AND Exp { $$=cnode("Exp", $1, 3, $1, $2, $3); }
    | Exp OR Exp { $$=cnode("Exp", $1, 3, $1, $2, $3); }
    | Exp RELOP Exp { $$=cnode("Exp", $1, 3, $1, $2, $3); }
    | Exp PLUS Exp { $$=cnode("Exp", $1, 3, $1, $2, $3); }
    | Exp MINUS Exp { $$=cnode("Exp", $1, 3, $1, $2, $3); }
    | Exp STAR Exp { $$=cnode("Exp", $1, 3, $1, $2, $3); }
    | Exp DIV Exp { $$=cnode("Exp", $1, 3, $1, $2, $3); }
    | LP Exp RP { $$=cnode("Exp", $1, 3, $1, $2, $3); }
    | MINUS Exp { $$=cnode("Exp", $1, 2, $1, $2); }
    | NOT Exp { $$=cnode("Exp", $1, 2, $1, $2); }
    | ID LP Args RP { $$=cnode("Exp", $1, 4, $1, $2, $3, $4); }
    | ID LP RP { $$=cnode("Exp", $1, 3, $1, $2, $3); }
    | Exp LB Exp RB { $$=cnode("Exp", $1, 4, $1, $2, $3, $4); }
    | Exp DOT ID { $$=cnode("Exp", $1, 3, $1, $2, $3); }
    | ID { $$=cnode("Exp", $1, 1, $1); }
    | INT { $$=cnode("Exp", $1, 1, $1); } 
    | FLOAT { $$=cnode("Exp", $1, 1, $1); }
    /* | error RP */
    ;
Args: Exp COMMA Args { $$=cnode("Args", $1, 3, $1, $2, $3); }
    | Exp { $$=cnode("Args", $1, 1, $1); }
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
    if (root)
        print_tree(root, 0);
    return 0;
}
