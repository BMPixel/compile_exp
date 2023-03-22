/* TODO:
- hex numbers
- scientific notation for floats
*/
%{
#include <stdio.h>
#include "syntax.tab.h"
void yyerror(const char *s);
%}

%option yylineno

integer 0|[1-9][0-9]*

%%

{integer}      { yylval.type_int = atoi(yytext); return INT; }

{integer}\.[0-9]+ { yylval.type_float = atof(yytext); return FLOAT; }

";"           { return SEMI; }

","           { return COMMA; }

"="           { return ASSIGNOP; }

">|<|>=|<=|==|!=" { return RELOP; }

"+"          { return PLUS; }

"-"          { return MINUS; }

"*"          { return STAR; }

"/"          { return DIV; }

"&&"         { return AND; }

"||"         { return OR; }

"."          { return DOT; }

"!"          { return NOT; }

int|float { return TYPE; }

"("          { return LP; }

")"          { return RP; }

"["          { return LB; }

"]"          { return RB; }

"{"          { return LC; }

"}"          { return RC; }

"struct"     { return STRUCT; }

"return"     { return RETURN; }

"if"         { return IF; }

"else"       { return ELSE; }

"while"      { return WHILE; }

[a-zA-Z_][a-zA-Z0-9_]* { return ID; }

[ \t\n]       /* ignore whitespace */

.           { yyerror("lexical error"); }

%%

void yyerror(const char *s) {
    if (strcmp(s, "syntax error") == 0) {
        fprintf(stderr, "Error type B at Line %d:\n", yylineno);
    } else if (strcmp (s, "lexical error") == 0) {
        fprintf(stderr, "Error type A at Line %d: Invalid Character \"%s\".\n", yylineno, yytext);
    } else {
        fprintf(stderr, "Error unknown at Line %d:\n", yylineno);
    }
    
}

int yywrap() {
    return 1;
}