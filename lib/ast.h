// AstNode of the AST
typedef struct AstNode
{
    char *name;
    int lineno;
    char *value_string;
    int is_terminal;
    struct AstNode *first_child;
    struct AstNode *next;
} AstNode;

AstNode *create_node(char *name, int lineno, int num_children, AstNode **children);

AstNode *create_terminal_node(char *name, int lineno, char *value);

void print_tree(AstNode *node, int level);