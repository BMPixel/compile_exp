// TODO: Exp
// TODO: Arg Check
// TODO: int x,y in Struct
#include "ast.h"
#include "analysis.h"
#include <iostream>
#include <string>
#include <cstring>
#include <vector>
using namespace std;

void debugCollectNode(AstNode *node, string &code);

void debugAstNode(const char *message, AstNode *node)
{
    if (node == NULL || node->is_terminal)
    {
        cout << message << ": NULL" << endl
             << "                                                  [ ]" << endl;
        return;
    }
    string code = "";
    debugCollectNode(node, code);
    cout << message << ": " << node->name << " @" << node->lineno << endl
         << "                                                  [ " << code << " ]" << endl;
}

void debugType(const char *message, SymbolType *type)
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

void debugVar(const char *message, SymbolVar *var)
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

void debugCollectNode(AstNode *node, string &code)
{
    if (node == NULL)
        return;
    if (strcmp(node->name, "ID") == 0)
        code += node->value_string;
    if (strcmp(node->name, "INT") == 0)
        code += node->value_string;
    if (strcmp(node->name, "FLOAT") == 0)
        code += node->value_string;
    if (strcmp(node->name, "TYPE") == 0)
        code += node->value_string;
    if (strcmp(node->name, "SEMI") == 0)
        code += ";";
    if (strcmp(node->name, "COMMA") == 0)
        code += ",";
    if (strcmp(node->name, "ASSIGNOP") == 0)
        code += "=";
    if (strcmp(node->name, "RELOP") == 0)
        code += node->value_string;
    if (strcmp(node->name, "PLUS") == 0)
        code += "+";
    if (strcmp(node->name, "MINUS") == 0)
        code += "-";
    if (strcmp(node->name, "STAR") == 0)
        code += "*";
    if (strcmp(node->name, "DIV") == 0)
        code += "/";
    if (strcmp(node->name, "AND") == 0)
        code += "&&";
    if (strcmp(node->name, "OR") == 0)
        code += "||";
    if (strcmp(node->name, "DOT") == 0)
        code += ".";
    if (strcmp(node->name, "NOT") == 0)
        code += "!";
    if (strcmp(node->name, "LP") == 0)
        code += "(";
    if (strcmp(node->name, "RP") == 0)
        code += ")";
    if (strcmp(node->name, "LB") == 0)
        code += "[";
    if (strcmp(node->name, "RB") == 0)
        code += "]";
    if (strcmp(node->name, "LC") == 0)
        code += "{";
    if (strcmp(node->name, "RC") == 0)
        code += "}";
    if (strcmp(node->name, "STRUCT") == 0)
        code += "struct";
    if (strcmp(node->name, "RETURN") == 0)
        code += "return";
    if (strcmp(node->name, "IF") == 0)
        code += "if";
    if (strcmp(node->name, "ELSE") == 0)
        code += "else";
    if (strcmp(node->name, "WHILE") == 0)
        code += "while";
    for (auto it = node->first_child; it != NULL; it = it->next)
        debugCollectNode(it, code);
}

vector<SymbolType *> type_table;
vector<SymbolVar *> var_table;
SymbolType *currentFuncContext = NULL;

bool assert(bool condition, int err_type, int lineno, string message)
{
    if (!condition)
        cout << "Error type " << err_type << " at Line " << lineno << ": " << message << endl;
    return condition;
}

bool testTypeEqual(SymbolType *t1, SymbolType *t2)
{
    if (t1 == NULL || t2 == NULL)
        return false;
    if (t1 == t2)
        return true;
    if (t1->kind != t2->kind)
        return false;
    if (t1->kind == SymbolType::BASIC)
    {
        return t1->basic == t2->basic;
    }
    else if (t1->kind == SymbolType::ARRAY)
    {
        return testTypeEqual(t1->array.elem, t2->array.elem);
    }
    else
    {
        return t1->name == t2->name;
    }
    return false;
}

SymbolType *searchTypeFromTable(string name)
{
    for (int i = 0; i < type_table.size(); i++)
    {
        if (type_table[i]->name == name)
            return type_table[i];
    }
    return NULL;
}

SymbolType *searchTypeFromStructure(string name, SymbolType *structureType)
{
    if (structureType == NULL)
        return NULL;
    if (structureType->kind != SymbolType::STRUCTURE)
        return NULL;
    for (int i = 0; i < structureType->structure.size(); i++)
    {
        if (structureType->structure[i]->name == name)
            return structureType->structure[i];
    }
    return NULL;
}

SymbolVar *searchVarFromTable(string name)
{
    for (int i = 0; i < var_table.size(); i++)
    {
        if (var_table[i]->name == name)
            return var_table[i];
    }
    return NULL;
}

bool tryPushTypeToTable(SymbolType *type, int lineno)
{
    if (!assert(type != NULL, -1, lineno, "Type is null"))
        return false;
    bool check = assert(type->name == "" || (searchTypeFromTable(type->name) == NULL && searchVarFromTable(type->name) == NULL), 16, lineno, "Redefined structure" + type->name);
    if (!check)
        return false;
    type_table.push_back(type);
    return true;
}

bool tryPushVarToTable(SymbolVar *var, int lineno)
{
    bool check;
    check = assert(var != NULL, -1, lineno, "Var is null");
    if (!check)
        return false;

    if (var->kind == SymbolVar::FUNCTION)
    {
        check = assert(var->name == "" || (searchTypeFromTable(var->name) == NULL && searchVarFromTable(var->name) == NULL), 4, lineno, "Redefined function " + var->name);
    }
    else
    {
        check = assert(var->name == "" || (searchTypeFromTable(var->name) == NULL && searchVarFromTable(var->name) == NULL), 3, lineno, "Redefined variable: " + var->name);
    }
    if (!check)
        return false;
    var_table.push_back(var);
    return true;
}

void handleProgram(AstNode *node)
{
    debugAstNode("Program", node);
    handleExtDefList(node->first_child);
}

void handleExtDefList(AstNode *node)
{
    debugAstNode("ExtDefList", node);
    if (node == NULL || node->is_terminal)
        return;
    handleExtDef(node->first_child);
    handleExtDefList(node->first_child->next);
}

void handleExtDef(AstNode *node)
{
    debugAstNode("ExtDef", node);
    if (node == NULL || node->is_terminal)
        return;
    SymbolType *type = NULL;
    if (strcmp(node->first_child->next->name, "SEMI") == 0)
    {
        handleSpecifier(node->first_child, type);
        debugType("Define a isolated Type", type);
        tryPushTypeToTable(type, node->lineno);
        return;
    }
    if (strcmp(node->first_child->next->name, "ExtDecList") == 0)
    {
        debugAstNode("Specifier", node->first_child);
        handleSpecifier(node->first_child, type);
        debugAstNode("ExtDecList", node->first_child->next);
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
        debugVar("Define a function", var);
        tryPushVarToTable(var, node->lineno);
        SymbolType *oldFuncContext = currentFuncContext;
        currentFuncContext = var->type;
        handleCompSt(node->first_child->next->next);
        currentFuncContext = oldFuncContext;
        return;
    }
}

void handleExtDecList(AstNode *node, SymbolType *type)
{
    debugAstNode("ExtDecList", node);
    if (node == NULL || node->is_terminal)
        return;
    SymbolVar *var = new SymbolVar;
    var->kind = SymbolVar::VARIABLE;
    var->type = type;
    handleVarDec(node->first_child, type);
    var->name = node->first_child->first_child->value_string;
    debugVar("Define a ext variable", var);
    tryPushVarToTable(var, node->lineno);
    if (node->first_child->next != NULL)
        handleExtDecList(node->first_child->next->next, type);
}

void handleSpecifier(AstNode *node, SymbolType *&type)
{
    debugAstNode("Specifier", node);
    if (node == NULL || node->is_terminal)
        return;
    if (strcmp(node->first_child->name, "TYPE") == 0)
    {
        type = new SymbolType{node->first_child->name, SymbolType::BASIC};
        type->kind = SymbolType::BASIC;
        type->name = node->first_child->value_string;
        if (strcmp(node->first_child->value_string, "int") == 0)
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
    debugAstNode("StructSpecifier", node);
    if (node == NULL || node->is_terminal)
        return;
    // case STRUCT Tag
    if (strcmp(node->first_child->next->name, "Tag") == 0)
    {
        string name;
        handleTag(node->first_child->next, name);
        type = searchTypeFromTable(name);
        if (!assert(type != NULL, 17, node->lineno, "Undefined structure: " + name))
        {
            type = new SymbolType{name, SymbolType::STRUCTURE};
        }
        return;
    }
    // case STRUCT OptTag LC DefList RC
    if (strcmp(node->first_child->next->name, "OptTag") == 0)
    {
        string name;
        handleOptTag(node->first_child->next, name);
        type = new SymbolType{name, SymbolType::STRUCTURE};
        debugType("Met StructSpecifier", type);
        handleStructDefList(node->first_child->next->next->next, type);
        return;
    }
}

void handleStructDefList(AstNode *node, SymbolType *type)
{
    debugAstNode("StructDefList", node);
    if (node == NULL || node->is_terminal)
        return;
    handleStructDef(node->first_child, type);
    handleStructDefList(node->first_child->next, type);
}

void handleStructDef(AstNode *node, SymbolType *type)
{
    debugAstNode("StructDef", node);
    if (node == NULL || node->is_terminal)
        return;
    SymbolType *field_type = NULL;
    handleSpecifier(node->first_child, field_type);
    handleStructDecList(node->first_child->next, field_type, type);
}

void handleStructDecList(AstNode *node, SymbolType *field_type, SymbolType *type)
{
    debugAstNode("StructDecList", node);
    if (node == NULL || node->is_terminal)
        return;
    handleStructDec(node->first_child, field_type, type);
    if (node->first_child->next != NULL)
        handleStructDecList(node->first_child->next->next, field_type, type);
}

void handleStructDec(AstNode *node, SymbolType *field_type, SymbolType *type)
{
    debugAstNode("StructDec", node);
    if (node == NULL || node->is_terminal)
        return;
    if (strcmp(node->first_child->name, "VarDec") == 0)
    {
        auto attr_type = new SymbolType{field_type->name, field_type->kind};
        handleVarDec(node->first_child, attr_type);
        SymbolType *defined_type = searchTypeFromStructure(attr_type->name, type);
        if (assert(defined_type == NULL, 15, node->lineno, "Redefined field: " + attr_type->name))
        {
            debugType("Add a attribute to structure", attr_type);
            type->structure.push_back(attr_type);
        }
        else
        {
            delete attr_type;
        }
    }
    assert(node->first_child->next == NULL, 15, node->lineno, "Structure attribute cannot have initial value");
}

void handleOptTag(AstNode *node, string &name)
{
    debugAstNode("OptTag", node);
    if (node == NULL || node->is_terminal)
        return;
    if (node->first_child != NULL)
        name = node->first_child->value_string;
}

void handleTag(AstNode *node, string &name)
{
    debugAstNode("Tag", node);
    if (node == NULL || node->is_terminal)
        return;
    name = node->first_child->value_string;
}

void handleVarDec(AstNode *node, SymbolType *&type)
{
    debugAstNode("VarDec", node);
    if (node == NULL || node->is_terminal)
        return;
    if (strcmp(node->first_child->name, "ID") == 0)
    {
        type->name = node->first_child->value_string;
        return;
    }
    if (strcmp(node->first_child->name, "VarDec") == 0)
    {
        SymbolType *inner_type = new SymbolType{type->name, type->kind};
        handleVarDec(node->first_child, inner_type);
        type->kind = SymbolType::ARRAY;
        type->array.size = atoi(node->first_child->next->next->value_string);
        type->array.elem = inner_type;
        type->name = inner_type->name;
        return;
    }
}

void handleFunDec(AstNode *node, SymbolVar *var)
{
    debugAstNode("FunDec", node);
    if (node == NULL || node->is_terminal)
        return;
    var->name = node->first_child->value_string;
    handleVarList(node->first_child->next->next, var);
}

void handleVarList(AstNode *node, SymbolVar *var)
{
    debugAstNode("VarList", node);
    if (node == NULL || node->is_terminal)
        return;
    handleParamDec(node->first_child, var);
    if (node->first_child != NULL && node->first_child->next != NULL)
        handleVarList(node->first_child->next->next, var);
}

void handleParamDec(AstNode *node, SymbolVar *var)
{
    debugAstNode("ParamDec", node);
    if (node == NULL || node->is_terminal)
        return;
    SymbolType *type = NULL;
    handleSpecifier(node->first_child, type);
    debugType("Add a param to function dec", type);
    var->params.push_back(type);
}

void handleCompSt(AstNode *node)
{
    debugAstNode("CompSt", node);
    if (node == NULL || node->is_terminal)
        return;
    handleDefList(node->first_child->next);
    handleStmtList(node->first_child->next->next);
}

void handleDefList(AstNode *node)
{
    debugAstNode("DefList", node);
    if (node == NULL || node->is_terminal)
        return;
    handleDef(node->first_child);
    handleDefList(node->first_child->next);
}

void handleDef(AstNode *node)
{
    debugAstNode("Def", node);
    if (node == NULL || node->is_terminal)
        return;
    SymbolType *type = NULL;
    handleSpecifier(node->first_child, type);
    SymbolType *type_copy = new SymbolType{type->name, type->kind};
    type_copy->basic = type->basic;
    type_copy->structure = type->structure;
    type_copy->array = type->array;
    handleDecList(node->first_child->next, type_copy);
}

void handleDecList(AstNode *node, SymbolType *&type)
{
    debugAstNode("DecList", node);
    if (node == NULL || node->is_terminal)
        return;
    handleDec(node->first_child, type);
    if (node->first_child->next != NULL)
        handleDecList(node->first_child->next->next, type);
}

void handleDec(AstNode *node, SymbolType *&type)
{
    debugAstNode("Dec", node);
    if (node == NULL || node->is_terminal)
        return;
    SymbolVar *var = new SymbolVar;
    var->type = type;
    if (strcmp(node->first_child->name, "VarDec") == 0)
    {
        handleVarDec(node->first_child, type);
        var->name = type->name;
        debugVar("Define a variable", var);
        tryPushVarToTable(var, node->lineno);
    }
    if (node->first_child->next != NULL)
        handleExp(node->first_child->next->next);
}

void handleStmtList(AstNode *node)
{
    debugAstNode("StmtList", node);
    if (node == NULL || node->is_terminal)
        return;
    handleStmt(node->first_child);
    handleStmtList(node->first_child->next);
}

void handleStmt(AstNode *node)
{
    debugAstNode("Stmt", node);
    if (node == NULL || node->is_terminal)
        return;
    if (strcmp(node->first_child->name, "Exp") == 0)
    {
        SymbolType *type = NULL;
        handleExp(node->first_child);
        return;
    }
    if (strcmp(node->first_child->name, "CompSt") == 0)
    {
        handleCompSt(node->first_child);
        return;
    }
    if (strcmp(node->first_child->name, "RETURN") == 0)
    {
        SymbolType *ret_type = handleExp(node->first_child->next);
        assert(testTypeEqual(ret_type, currentFuncContext), 8, node->lineno, "Return type mismatched");
        return;
    }
    if (strcmp(node->first_child->name, "IF") == 0)
    {
        SymbolType *type = NULL;
        handleExp(node->first_child->next->next);
        handleStmt(node->first_child->next->next->next->next);
        if (node->first_child->next->next->next->next->next != NULL)
            handleStmt(node->first_child->next->next->next->next->next->next);
        return;
    }
    if (strcmp(node->first_child->name, "WHILE") == 0)
    {
        SymbolType *type = NULL;
        handleExp(node->first_child->next->next);
        handleStmt(node->first_child->next->next->next->next);
        return;
    }
}

SymbolType *handleExp(AstNode *node)
{
    // return 0 if it is a left value and 1 if it is a right value
    if (node == NULL || node->is_terminal)
        return NULL;
    if (strcmp(node->first_child->name, "ID") == 0)
    {
        if (node->first_child->next == NULL)
        {
            debugAstNode("Exp->ID", node);
            auto definedVars = searchVarFromTable(node->first_child->value_string);
            bool is_defined = assert(definedVars != NULL, 1, node->lineno, "Undefined variable " + string(node->first_child->value_string));
            if (is_defined)
            {
                definedVars->type->isLeftValue = true;
                return definedVars->type;
            }
            return 0;
        }
        if (strcmp(node->first_child->next->name, "LP") == 0)
        {
            debugAstNode("Exp->FUNC", node);
            // ID LP Args RP
            auto var = searchVarFromTable(node->first_child->value_string);
            if (var == NULL)
            {
                assert(false, 2, node->lineno, "Undefined function " + string(node->first_child->value_string));
                return NULL;
            }
            else
            {
                assert(var->kind == SymbolVar::FUNCTION, 11, node->lineno, var->name + " is not a function");
            }
            handleArgs(node->first_child->next->next, var);
            return var->type;
        }
        if (strcmp(node->first_child->next->name, "LB") == 0)
        {
            debugAstNode("Exp->ARRAY", node);
            // Exp LB Exp RB
            auto type = handleExp(node->first_child->next->next);
            assert(type->kind == SymbolType::BASIC && type->basic == 0, 12, node->lineno, "Array index should be an integer");
            // TODO: this is hard to implement
            return NULL;
        }
    }
    if (strcmp(node->first_child->name, "INT") == 0)
    {
        debugAstNode("Exp->INT", node);
        auto type = new SymbolType{"int", SymbolType::BASIC};
        type->basic = 0;
        return type;
    }
    if (strcmp(node->first_child->name, "FLOAT") == 0)
    {
        debugAstNode("Exp->FLOAT", node);
        auto type = new SymbolType{"float", SymbolType::BASIC};
        type->basic = 1;
        return type;
    }
    if (strcmp(node->first_child->next->name, "ASSIGNOP") == 0)
    {
        debugAstNode("Exp->ASSIGN", node);
        auto typeleft = handleExp(node->first_child);
        auto typeright = handleExp(node->first_child->next->next);
        if (!typeleft || !typeright)
            return NULL;
        assert(typeleft->kind == typeright->kind, 5, node->lineno, "Type mismatched for assignment");
        if (typeleft->kind == SymbolType::BASIC)
            assert(typeleft->basic == typeright->basic, 5, node->lineno, "Type mismatched for assignment");
        return typeleft;
    }
    if (strcmp(node->first_child->name, "LP") == 0)
    {
        debugAstNode("Exp->()", node);
        return handleExp(node->first_child->next);
    }
    if (strcmp(node->first_child->name, "MINUS") == 0)
    {
        debugAstNode("Exp->-", node);
        return handleExp(node->first_child->next);
    }
    if (strcmp(node->first_child->name, "NOT") == 0)
    {
        debugAstNode("Exp->!", node);
        return handleExp(node->first_child->next);
    }
    if (strcmp(node->first_child->next->name, "DOT") == 0)
    {
        debugAstNode("Exp->.", node);
        auto type = handleExp(node->first_child);
        assert(type->kind == SymbolType::STRUCTURE, 13, node->lineno, "Illegal use of \".\"");
        string field_name = node->first_child->next->next->value_string;
        SymbolType *field_type = searchTypeFromStructure(field_name, type);
        assert(field_type != NULL, 14, node->lineno, "Non-existent field " + field_name);
        return field_type;
    }
    // Everything else, including Exp OP Exp
    debugAstNode("Exp->OP", node);
    auto typeleft = handleExp(node->first_child);
    auto typeright = handleExp(node->first_child->next->next);
    if (!typeleft || !typeright)
        return NULL;
    if (typeleft->kind == SymbolType::BASIC && typeright->kind == SymbolType::BASIC)
    {
        assert(typeleft->basic == typeright->basic, 7, node->lineno, "Type mismatched for operands");
        return typeleft;
    }
    else
    {
        assert(false, 7, node->lineno, "Type mismatched for operands");
        return NULL;
    }
}

void handleArgs(AstNode *node, SymbolVar *&var)
{
    debugAstNode("Args", node);
}