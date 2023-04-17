#include "ast.h"
#include "analysis.h"
#include <iostream>
#include <string>
#include <vector>
using namespace std;

vector<SymbolType *> type_table;
vector<SymbolVar *> var_table;
vector<string> err_messages;

SymbolType *searchType(string name)
{
    for (int i = 0; i < type_table.size(); i++)
    {
        if (type_table[i]->name == name)
            return type_table[i];
    }
    return NULL;
}

SymbolVar *searchVar(string name)
{
    for (int i = 0; i < var_table.size(); i++)
    {
        if (var_table[i]->name == name)
            return var_table[i];
    }
    return NULL;
}

void handleProgram(AstNode *node)
{
    handleExtDefList(node->first_child);
}

void handleExtDefList(AstNode *node)
{
    if (node == NULL)
        return;
    handleExtDef(node->first_child);
    handleExtDefList(node->first_child->next);
}

void handleExtDef(AstNode *node)
{
    if (node == NULL)
        return;
    SymbolType *type = NULL;
    if (node->first_child->next->name == "SEMI")
    {
        handleSpecifier(node->first_child, type);
        type_table.push_back(type);
        return;
    }
    if (node->first_child->next->name == "ExtDecList")
    {
        handleSpecifier(node->first_child, type);
        handleExtDecList(node->first_child->next, type);
        return;
    }
    if (node->first_child->next->name == "FunDec")
    {
        handleSpecifier(node->first_child, type);
        SymbolVar *var = new SymbolVar;
        var->kind = SymbolVar::FUNCTION;
        var->type = type;
        handleFunDec(node->first_child->next, var);
        var_table.push_back(var);
        handleCompSt(node->first_child->next->next);
        return;
    }
}

void handleExtDecList(AstNode *node, SymbolType *type)
{
    if (node == NULL)
        return;
    SymbolVar *var = new SymbolVar;
    var->kind = SymbolVar::VARIABLE;
    var->type = type;
    handleVarDec(node->first_child, type);
    var->name = node->first_child->first_child->value;
    var_table.push_back(var);
    if (node->first_child->next != NULL)
        handleExtDecList(node->first_child->next->next, type);
}

void handleSpecifier(AstNode *node, SymbolType *&type)
{
    if (node == NULL)
        return;
    if (node->first_child->name == "TYPE")
    {
        type = new SymbolType;
        type->kind = SymbolType::BASIC;
        if (node->first_child->value == "int")
            type->u.basic = 0;
        else
            type->u.basic = 1;
        return;
    }
    if (node->first_child->name == "StructSpecifier")
    {
        handleStructSpecifier(node->first_child, type);
        return;
    }
}

void handleStructSpecifier(AstNode *node, SymbolType *&type)
{
    if (node == NULL)
        return;
    // case STRUCT Tag
    if (node->first_child->next->name == "Tag")
    {
        string name;
        handleTag(node->first_child->next, name);
        type = searchType(name);
        return;
    }
    // case STRUCT OptTag LC DefList RC
    if (node->first_child->next->name == "OptTag")
    {
        string name;
        handleOptTag(node->first_child->next, name);
        type = new SymbolType;
        type->name = name;
        type->kind = SymbolType::STRUCTURE;
        type_table.push_back(type);
        handleStructDefList(node->first_child->next->next->next, type);
        return;
    }
}

void handleStructDefList(AstNode *node, SymbolType *type)
{
    if (node == NULL)
        return;
    handleStructDef(node->first_child, type);
    handleStructDefList(node->first_child->next, type);
}

void handleStructDef(AstNode *node, SymbolType *type)
{
    if (node == NULL)
        return;
    SymbolType *field_type = NULL;
    handleSpecifier(node->first_child, field_type);
    handleStructDecList(node->first_child->next, field_type, type);
}

void handleStructDecList(AstNode *node, SymbolType *field_type, SymbolType *type)
{
    if (node == NULL)
        return;
    handleStructDec(node->first_child, field_type, type);
    if (node->first_child->next != NULL)
        handleStructDecList(node->first_child->next->next, field_type, type);
}

void handleStructDec(AstNode *node, SymbolType *field_type, SymbolType *type)
{
    if (node == NULL)
        return;
    string name = node->first_child->first_child->value;
    SymbolType *var_type = new SymbolType;
    var_type->kind = field_type->kind;
    var_type.u = field_type->u;
    var_type->name = name;
    type->u.structure.push_back(var_type);
}

void handleOptTag(AstNode *node, string &name)
{
    if (node == NULL)
        return;
    if (node->first_child != NULL)
        name = node->first_child->value;
}

void handleTag(AstNode *node, string &name)
{
    if (node == NULL)
        return;
    name = node->first_child->next->value;
}

void handleVarDec(AstNode *node, SymbolType *&type)
{
    if (node == NULL)
        return;
    if (node->first_child->name == "ID")
    {
        type->name = node->first_child->value;
        return;
    }
    if (node->first_child->name == "VarDec")
    {
        handleVarDec(node->first_child, type);
        type->kind = SymbolType::ARRAY;
        type->u.array.size = atoi(node->first_child->next->next->value.c_str());
        return;
    }
}

void handleFunDec(AstNode *node, SymbolVar *var)
{
    if (node == NULL)
        return;
    var->name = node->first_child->value;
    handleVarList(node->first_child->next->next, var);
}

void handleVarList(AstNode *node, SymbolVar *var)
{
    if (node == NULL)
        return;
    handleParamDec(node->first_child, var);
    if (node->first_child->next != NULL)
        handleVarList(node->first_child->next->next, var);
}

void handleParamDec(AstNode *node, SymbolVar *var)
{
    if (node == NULL)
        return;
    SymbolType *type = NULL;
    handleSpecifier(node->first_child, type);
    var->params.push_back(type);
}

void handleCompSt(AstNode *node)
{
    if (node == NULL)
        return;
    handleDefList(node->first_child->next);
    handleStmtList(node->first_child->next->next);
}

void handleDefList(AstNode *node)
{
    if (node == NULL)
        return;
    handleDef(node->first_child);
    handleDefList(node->first_child->next);
}

void handleDef(AstNode *node)
{
    if (node == NULL)
        return;
    SymbolType *type = NULL;
    handleSpecifier(node->first_child, type);
    handleDecList(node->first_child->next, type);
}

void handleDecList(AstNode *node, SymbolType *&type)
{
    if (node == NULL)
        return;
    handleDec(node->first_child, type);
    if (node->first_child->next != NULL)
        handleDecList(node->first_child->next->next, type);
}

void handleDec(AstNode *node, SymbolType *&type)
{
    if (node == NULL)
        return;
    SymbolVar *var = new SymbolVar;
    var->type = type;
    var->name = node->first_child->first_child->value;
    var_table.push_back(var);
    if (node->first_child->next != NULL)
        handleExp(node->first_child->next->next);
}

void handleStmtList(AstNode *node)
{
    if (node == NULL)
        return;
    handleStmt(node->first_child);
    handleStmtList(node->first_child->next);
}

void handleStmt(AstNode *node)
{
    cout << "handleStmt" << endl;
}

void handleExp(AstNode *node)
{
    cout << "handleExp" << endl;
}

void handleArgs(AstNode *node)
{
    cout << "handleArgs" << endl;
}