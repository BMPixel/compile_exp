// Node of the AST
typedef struct AstNode
{
    char *name;
    int lineno;
    char *value_string;
    int is_terminal;
    struct AstNode *first_child;
    struct AstNode *next_sibling;
} AstNode;

AstNode *create_node(char *name, int lineno, int num_children, AstNode **children);

AstNode *create_terminal_node(char *name, int lineno, char *value);

void print_tree(AstNode *node, int level);

typedef struct
{
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
        SymbolFieldList *structure;
    } u;
} SymbolType;

typedef struct
{
    char *name;
    SymbolType *type;
    SymbolFieldList *next;
} SymbolFieldList;