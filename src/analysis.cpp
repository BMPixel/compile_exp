#include "ast.h"
#include "analysis.h"
#include <iostream>
#include <string>
#include <cstring>
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

void logAstNode(const char *message, AstNode *node)
{
    if (node == NULL)
    {
        cout << message << ": NULL" << endl;
        return;
    }
    if (node->value_string == NULL)
        cout << message << ": " << node->name << " @line " << node->lineno << endl;
    else
        cout << message << ": " << node->value_string << " @line " << node->lineno << endl;
}

void logType(const char *message, SymbolType *type)
{
    if (type == NULL)
    {
        cout << message << ": NULL" << endl;
        return;
    }
    string kind_name = "";
    switch (type->kind)
    {
    case SymbolType::BASIC:
        kind_name = "BASIC";
        break;
    case SymbolType::ARRAY:
        kind_name = "ARRAY";
        break;
    case SymbolType::STRUCTURE:
        kind_name = "STRUCTURE";
        break;
    }
    cout << "-------" << message << ": " << type->name << " (" << kind_name << ")" << endl;
}

void logVar(const char *message, SymbolVar *var)
{
    if (var == NULL)
    {
        cout << message << ": NULL" << endl;
        return;
    }
    string kind_name = "";
    switch (var->kind)
    {
    case SymbolVar::VARIABLE:
        kind_name = "VARIABLE";
        break;
    case SymbolVar::FUNCTION:
        kind_name = "FUNCTION";
        break;
    }
    cout << "-----" << message << ": " << var->name << " (" << kind_name << ")" << endl;
}

void handleProgram(AstNode *node)
{
    logAstNode("Program", node);
    handleExtDefList(node->first_child);
}

void handleExtDefList(AstNode *node)
{
    logAstNode("ExtDefList", node);
    if (node == NULL)
        return;
    handleExtDef(node->first_child);
    handleExtDefList(node->first_child->next);
}

void handleExtDef(AstNode *node)
{
    logAstNode("ExtDef", node);
    if (node == NULL)
        return;
    SymbolType *type = NULL;
    if (strcmp(node->first_child->next->name, "SEMI") == 0)
    {
        handleSpecifier(node->first_child, type);
        logType("ExtDef: type", type);
        type_table.push_back(type);
        return;
    }
    if (strcmp(node->first_child->next->name, "ExtDecList") == 0)
    {
        handleSpecifier(node->first_child, type);
        handleExtDecList(node->first_child->next, type);
        return;
    }
    if (strcmp(node->first_child->next->name, "FunDec") == 0)
    {
        handleSpecifier(node->first_child, type);
        SymbolVar *var = new SymbolVar;
        var->kind = SymbolVar::FUNCTION;
        var->type = type;
        handleFunDec(node->first_child->next, var);
        logVar("ExtDef: var", var);
        var_table.push_back(var);
        handleCompSt(node->first_child->next->next);
        return;
    }
}

void handleExtDecList(AstNode *node, SymbolType *type)
{
    logAstNode("ExtDecList", node);
    if (node == NULL)
        return;
    SymbolVar *var = new SymbolVar;
    var->kind = SymbolVar::VARIABLE;
    var->type = type;
    handleVarDec(node->first_child, type);
    var->name = node->first_child->first_child->value_string;
    logVar("ExtDecList: var", var);
    var_table.push_back(var);
    if (node->first_child->next != NULL)
        handleExtDecList(node->first_child->next->next, type);
}

void handleSpecifier(AstNode *node, SymbolType *&type)
{
    logAstNode("Specifier", node);
    if (node == NULL)
        return;
    if (strcmp(node->first_child->name, "TYPE") == 0)
    {
        type = new SymbolType{node->first_child->name, SymbolType::BASIC};
        type->kind = SymbolType::BASIC;
        if (node->first_child->name == "int")
            type->basic = 0;
        else
            type->basic = 1;
        return;
    }
    if (strcmp(node->first_child->name, "StructSpecifier") == 0)
    {
        handleStructSpecifier(node->first_child, type);
        return;
    }
}

void handleStructSpecifier(AstNode *node, SymbolType *&type)
{
    logAstNode("StructSpecifier", node);
    if (node == NULL)
        return;
    // case STRUCT Tag
    if (strcmp(node->first_child->next->name, "Tag") == 0)
    {
        string name;
        handleTag(node->first_child->next, name);
        type = searchType(name);
        return;
    }
    // case STRUCT OptTag LC DefList RC
    if (strcmp(node->first_child->next->name, "OptTag") == 0)
    {
        string name;
        handleOptTag(node->first_child->next, name);
        type = new SymbolType{name, SymbolType::STRUCTURE};
        logType("StructSpecifier: type", type);
        type_table.push_back(type);
        handleStructDefList(node->first_child->next->next->next, type);
        return;
    }
}

void handleStructDefList(AstNode *node, SymbolType *type)
{
    logAstNode("StructDefList", node);
    if (node == NULL)
        return;
    handleStructDef(node->first_child, type);
    handleStructDefList(node->first_child->next, type);
}

void handleStructDef(AstNode *node, SymbolType *type)
{
    logAstNode("StructDef", node);
    if (node == NULL)
        return;
    SymbolType *field_type = NULL;
    handleSpecifier(node->first_child, field_type);
    handleStructDecList(node->first_child->next, field_type, type);
}

void handleStructDecList(AstNode *node, SymbolType *field_type, SymbolType *type)
{
    logAstNode("StructDecList", node);
    if (node == NULL)
        return;
    handleStructDec(node->first_child, field_type, type);
    if (node->first_child->next != NULL)
        handleStructDecList(node->first_child->next->next, field_type, type);
}

void handleStructDec(AstNode *node, SymbolType *field_type, SymbolType *type)
{
    logAstNode("StructDec", node);
    if (node == NULL)
        return;
    string name = node->first_child->first_child->value_string;
    SymbolType *var_type = new SymbolType{field_type->name, field_type->kind};
    var_type->kind = field_type->kind;
    var_type->basic = field_type->basic;
    var_type->array = field_type->array;
    var_type->structure = field_type->structure;
    var_type->name = name;
    logType("StructDec: var_type", var_type);
    type->structure.push_back(var_type);
}

void handleOptTag(AstNode *node, string &name)
{
    logAstNode("OptTag", node);
    if (node == NULL)
        return;
    if (node->first_child != NULL)
        name = node->first_child->value_string;
}

void handleTag(AstNode *node, string &name)
{
    logAstNode("Tag", node);
    if (node == NULL)
        return;
    name = node->first_child->next->value_string;
}

void handleVarDec(AstNode *node, SymbolType *&type)
{
    logAstNode("VarDec", node);
    if (node == NULL)
        return;
    if (strcmp(node->first_child->name, "ID") == 0)
    {
        type->name = node->first_child->value_string;
        return;
    }
    if (strcmp(node->first_child->name, "VarDec") == 0)
    {
        handleVarDec(node->first_child, type);
        type->kind = SymbolType::ARRAY;
        type->array.size = atoi(node->first_child->next->next->name);
        return;
    }
}

void handleFunDec(AstNode *node, SymbolVar *var)
{
    logAstNode("FunDec", node);
    if (node == NULL)
        return;
    var->name = node->first_child->value_string;
    handleVarList(node->first_child->next->next, var);
}

void handleVarList(AstNode *node, SymbolVar *var)
{
    logAstNode("VarList", node);
    if (node == NULL)
        return;
    handleParamDec(node->first_child, var);
    if (node->first_child != NULL && node->first_child->next != NULL)
        handleVarList(node->first_child->next->next, var);
}

void handleParamDec(AstNode *node, SymbolVar *var)
{
    logAstNode("ParamDec", node);
    if (node == NULL)
        return;
    SymbolType *type = NULL;
    handleSpecifier(node->first_child, type);
    logType("ParamDec: type", type);
    var->params.push_back(type);
}

void handleCompSt(AstNode *node)
{
    logAstNode("CompSt", node);
    if (node == NULL)
        return;
    handleDefList(node->first_child->next);
    handleStmtList(node->first_child->next->next);
}

void handleDefList(AstNode *node)
{
    logAstNode("DefList", node);
    if (node == NULL)
        return;
    handleDef(node->first_child);
    handleDefList(node->first_child->next);
}

void handleDef(AstNode *node)
{
    logAstNode("Def", node);
    if (node == NULL)
        return;
    SymbolType *type = NULL;
    handleSpecifier(node->first_child, type);
    handleDecList(node->first_child->next, type);
}

void handleDecList(AstNode *node, SymbolType *&type)
{
    logAstNode("DecList", node);
    if (node == NULL)
        return;
    handleDec(node->first_child, type);
    if (node->first_child->next != NULL)
        handleDecList(node->first_child->next->next, type);
}

void handleDec(AstNode *node, SymbolType *&type)
{
    logAstNode("Dec", node);
    if (node == NULL)
        return;
    SymbolVar *var = new SymbolVar;
    var->type = type;
    var->name = node->first_child->first_child->value_string;
    logVar("Dec: var", var);
    var_table.push_back(var);
    if (node->first_child->next != NULL)
        handleExp(node->first_child->next->next, type);
}

void handleStmtList(AstNode *node)
{
    logAstNode("StmtList", node);
    if (node == NULL)
        return;
    handleStmt(node->first_child);
    handleStmtList(node->first_child->next);
}

void handleStmt(AstNode *node)
{
    logAstNode("Stmt", node);
    if (node == NULL)
        return;
    if (strcmp(node->first_child->name, "Exp") == 0)
    {
        SymbolType *type = NULL;
        handleExp(node->first_child, type);
        return;
    }
    if (strcmp(node->first_child->name, "CompSt") == 0)
    {
        handleCompSt(node->first_child);
        return;
    }
    if (strcmp(node->first_child->name, "RETURN") == 0)
    {
        SymbolType *type = NULL;
        handleExp(node->first_child->next, type);
        return;
    }
    if (strcmp(node->first_child->name, "IF") == 0)
    {
        SymbolType *type = NULL;
        handleExp(node->first_child->next->next, type);
        handleStmt(node->first_child->next->next->next->next);
        if (node->first_child->next->next->next->next->next != NULL)
            handleStmt(node->first_child->next->next->next->next->next->next);
        return;
    }
    if (strcmp(node->first_child->name, "WHILE") == 0)
    {
        SymbolType *type = NULL;
        handleExp(node->first_child->next->next, type);
        handleStmt(node->first_child->next->next->next->next);
        return;
    }
}

void handleExp(AstNode *node, SymbolType *&type)
{
    logAstNode("Exp", node);
}

void handleArgs(AstNode *node, vector<SymbolType *> param)
{
    logAstNode("Args", node);
}