#ifndef ANALYSIS_H
#define ANALYSIS_H
#include <vector>
#include <string>
#include "ast.h"
using namespace std;
struct SymbolType
{
    string name;
    enum
    {
        BASIC,
        ARRAY,
        STRUCTURE,
    } kind;
    union
    {
        int basic;
        struct
        {
            SymbolType *elem;
            int size;
        } array;
        vector<SymbolType *> structure;
    } u;
};

struct SymbolVar
{
    enum
    {
        VARIABLE,
        FUNCTION
    } kind;
    string name;
    SymbolType *type;
    vector<SymbolType *> params;
};

void handleProgram(AstNode *node);
void handleExtDefList(AstNode *node);
void handleExtDef(AstNode *node);
void handleExtDecList(AstNode *node, SymbolType *type);
void handleSpecifier(AstNode *node, SymbolType *&type);
void handleStructSpecifier(AstNode *node, SymbolType *&type);
void handleStructDefList(AstNode *node, SymbolType *type);
void handleStructDef(AstNode *node, SymbolType *type);
void handleStructDecList(AstNode *node, SymbolType *field_type, SymbolType *type);
void handleStructDec(AstNode *node, SymbolType *field_type, SymbolType *type);
void handleOptTag(AstNode *node, string &name);
void handleTag(AstNode *node, string &name);
void handleVarDec(AstNode *node, SymbolType *&type);
void handleFunDec(AstNode *node, SymbolVar *var);
void handleVarList(AstNode *node, SymbolVar *var);
void handleParamDec(AstNode *node, SymbolVar *var);
void handleCompSt(AstNode *node);
void handleStmtList(AstNode *node);
void handleStmt(AstNode *node);
void handleDefList(AstNode *node);
void handleDef(AstNode *node);
void handleDecList(AstNode *node, SymbolType *&type);
void handleDec(AstNode *node, SymbolType *&type);
void handleExp(AstNode *node, SymbolType *&type);
void handleArgs(AstNode *node, vector<SymbolType *> param);

#endif