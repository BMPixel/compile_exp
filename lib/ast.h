#ifndef ASTNODE_H
#define ASTNODE_H
#include <initializer_list>
// Node of the AST
struct AstNode
{
    char *name;
    int lineno;
    char *value_string;
    int is_terminal;
    struct AstNode *first_child;
    struct AstNode *next;
};

AstNode *create_node(const char *name, int lineno, int num_children, std::initializer_list<AstNode *> children);

AstNode *create_terminal_node(char *name, int lineno, char *value);

void print_tree(AstNode *node, int level);

#endif