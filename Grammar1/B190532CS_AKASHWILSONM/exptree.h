typedef struct tnode{
    int val;        //value for constants
    char *nodetype; //type of node (if, else, id, connector(c), =, + ..)
    char *varname;  //name of the variable
    int binding;
    struct tnode *left, *right; //left and right subtrees
}tnode;

struct Gsymbol{
    char *name;
    int binding;
    struct Gsymbol *next;
};


struct tnode* makeLeaf(int n);

struct tnode* makeID(char *c);

struct tnode* makeInternal(char c, struct tnode *left, struct tnode *right);

struct tnode* makeIf(struct tnode *exp, struct tnode *ifnode, struct tnode *elsenode);

int evaluate(struct tnode *node);

int codegen(struct tnode *node);

void createfile(struct tnode *node);

void unlabeled();