#ifndef ANALYSIS_H
#define ANALYSIS_H
#include <vector>
#include <string>
#include "ast.h"
using namespace std;

struct SymbolType
{
    string name;
    enum Kind
    {
        BASIC,
        ARRAY,
        STRUCTURE,
    } kind;

    int basic;
    struct
    {
        SymbolType *elem;
        int size;
    } array;

    vector<SymbolType *> structure;

    // 定义构造函数
    SymbolType(const string &name, Kind kind)
        : name(name), kind(kind)
    {
        // 根据 kind 的值初始化其他成员变量
        switch (kind)
        {
        case BASIC:
            basic = 0;
            break;
        case ARRAY:
            array.elem = nullptr;
            array.size = 0;
            break;
        case STRUCTURE:
            // 对于 STRUCTURE 类型，不需要在此处进行初始化，因为 vector 已经有默认构造函数
            break;
        }
    }

    // 默认析构函数足够满足需求
};
;

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