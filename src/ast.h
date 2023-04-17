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

AstNode *create_node(const char *name, int lineno, int num_children, AstNode **children);

AstNode *create_terminal_node(const char *name, int lineno, const char *value);

void print_tree(AstNode *node, int level);