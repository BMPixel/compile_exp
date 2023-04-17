// Node of the AST
typedef struct Node {
    char *name;
    int lineno;
    char *value_string;
    int is_terminal;
    struct Node *first_child;
    struct Node *next_sibling;
} Node;

Node* create_node(char *name, int lineno, int num_children, Node **children);

Node* create_terminal_node(char * name, int lineno, char* value);

void print_tree(Node *node, int level);