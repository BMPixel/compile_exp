/* TODO:
- hex numbers
- scientific notation for floats
- get their values
*/
%{
#include <stdio.h>
#include "ast.h"
#include "syntax.tab.h"
#define TNODE(n, v) yylval.node_ptr = create_terminal_node(n, yylineno, v)
void yyerror(const char *s);
extern Node* root;
extern int error_count;
%}

%option yylineno

%%

0|[1-9][0-9]*|0x[0-9a-fA-F]+|0[1-7][0-7]+     { TNODE("INT", yytext); return INT; }

([0-9]+\.[0-9]*|\.[0-9]+)([Ee][-+]?[0-9]+)|[0-9]+\.[0-9]+ { TNODE("FLOAT", yytext); return FLOAT; }

";"           { TNODE("SEMI", NULL); return SEMI; }

","           { TNODE("COMMA", NULL); return COMMA; }

"="           { TNODE("ASSIGNOP", NULL); return ASSIGNOP; }

">"|"<"|">="|"<="|"=="|"!=" { TNODE("RELOP", yytext); return RELOP; }

"+"          { TNODE("PLUS", NULL); return PLUS; }

"-"          { TNODE("MINUS", NULL); return MINUS; }

"*"          { TNODE("STAR", NULL); return STAR; }

"/"          { TNODE("DIV", NULL); return DIV; } 

"&&"         { TNODE("AND", NULL); return AND; }

"||"         { TNODE("OR", NULL); return OR; }

"."          { TNODE("DOT", NULL); return DOT; }

"!"          { TNODE("NOT", NULL); return NOT; }

int|float { TNODE("TYPE", yytext); return TYPE; }

"("          { TNODE("LP", NULL); return LP; }

")"          { TNODE("RP", NULL); return RP; }

"["          { TNODE("LB", NULL); return LB; }

"]"          { TNODE("RB", NULL); return RB; }

"{"          { TNODE("LC", NULL); return LC; }

"}"          { TNODE("RC", NULL); return RC; }

"struct"     { TNODE("STRUCT", NULL); return STRUCT; }

"return"     { TNODE("RETURN", NULL); return RETURN; }

"if"         { TNODE("IF", NULL); return IF; }

"else"       { TNODE("ELSE", NULL); return ELSE; }

"while"      { TNODE("WHILE", NULL); return WHILE; }

"/*"       {
            char c, prev = 0;
            while((c = input()) != 0) {
                if(c == '/' && prev == '*') break;
                prev = c;
            }
         }
"//"       {
            while(input() != '\n');
         }

[a-zA-Z_][a-zA-Z0-9_]* { TNODE("ID", yytext); return ID; }

[ \t\n]       /* ignore whitespace */

.           { yyerror("lexical error"); }

%%

void yyerror(const char *s) {
    error_count++;
    root = NULL;
    if (strcmp(s, "syntax error") == 0) { 
        fprintf(stderr, "Error type B at Line %d: syntax error\n", yylineno);
    } else if (strcmp (s, "lexical error") == 0) {
        fprintf(stderr, "Error type A at Line %d: Invalid Character \"%s\".\n", yylineno, yytext);
    } else {
        fprintf(stderr, "Error unknown at Line %d:\n", yylineno);
    }
    
}

int yywrap() {
    return 1;
}