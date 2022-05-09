%{
    #include <stdio.h>
    #include "exptree.h"
    #include "exptree.c"
    int yylex(void);

    void yyerror(char const *s);

    void display(struct tnode *node);
%}

%union{
    struct tnode *node;
}

%type <node> E start stmt slist inpstmt outstmt asgstmt ifstmt whilestmt
%token FUN
%token <node> NUM
%token <node> VAR
%token READ
%token WRITE
%token NE
%token EE
%token GE
%token LE
%token IF
%token ELSE
%token WHILE
%token DO
%left '-'
%left '+'
%left '*'
%nonassoc '<'
%nonassoc '>'
%nonassoc '='
%nonassoc LE
%nonassoc GE
%nonassoc EE
%nonassoc NE

%%

start       :  FUN '(' ')' '{' slist '}'                        {   
                                                                    //display($5);
                                                                    //printf("test");
                                                                    createfile($5);
                                                                    return 0;
                                                                }
            |   FUN '(' ')' '{' '}'                            {
                                                                    createfile(NULL);
                                                                    return 0;}

slist       :   slist stmt                                      {
                                                                    $$ = makeInternal('c', $1, $2);
                                                                    //printf("created node\n");
                                                                }
            |   stmt                                            {$$ = $1;}

stmt        :   inpstmt ';'                                     {$$ = $1;}
            |   outstmt ';'                                     {$$ = $1;}
            |   asgstmt ';'                                     {$$ = $1;}
            |   ifstmt
            |   whilestmt ';'

inpstmt     :   READ '(' VAR ')'                                 {
                                                                    $$ = makeInternal('r', $3, NULL);
                                                                    //printf("created read node\n");
                                                                }

outstmt     :   WRITE '(' E ')'                                 {
                                                                    $$ = makeInternal('w', $3, NULL);
                                                                    //printf("created write node\n");
                                                                }

asgstmt     :   VAR '=' E                                       {   
                                                                    $$ = makeInternal('=', $1, $3);
                                                                    //printf("created = node\n");
                                                                }

ifstmt      :   IF '(' E ')' '{' slist '}' ELSE '{' slist '}'   {   $$ = makeIf($3, $6, $10);
                                                                    //printf("if else node created..\n");
                                                                } 
            |   IF '(' E ')' '{' slist '}'                      {   $$ = makeIf($3, $6, NULL);
                                                                    //printf("If node created\n");
                                                                }

whilestmt   :   DO '{' slist '}' WHILE '(' E ')'                {   $$ = makeInternal('d', $7, $3);
                                                                    //printf("do while node created..\n");
                                                                }

E           :   E '+' E                                         {   
                                                                    $$ = makeInternal('+', $1, $3);
                                                                    //printf("created + node\n");
                                                                }
            |   E '*' E                                         {   
                                                                    $$ = makeInternal('*', $1, $3);
                                                                    //printf("created * node\n");
                                                                }
            |   E '-' E                                         {   
                                                                    $$ = makeInternal('-', $1, $3);
                                                                    //printf("created - node\n");
                                                                }
            |   E '>' E                                         {
                                                                    $$ = makeInternal('>', $1, $3);
                                                                    //printf("created > node\n");
                                                                }
            |   E '<' E                                         {
                                                                    $$ = makeInternal('<', $1, $3);
                                                                    //printf("created < node..\n");
                                                                }
            |   E EE E                                          {
                                                                    $$ = makeInternal('e', $1, $3);
                                                                    //printf("created e(==) node..\n");
                                                                }
            |   E NE E                                          {
                                                                    $$ = makeInternal('!', $1, $3);
                                                                    //printf("created !(!=) node..\n");
                                                                }
            |   E LE E                                          {
                                                                    $$ = makeInternal('l', $1, $3);
                                                                    //printf("created l(<=) node..\n");
                                                                }
            |   E GE E                                          {
                                                                    $$ = makeInternal('g', $1, $3);
                                                                    //printf("created g(>=) node..\n");
                                                                }
        
                                        
            |   '(' E ')'                                       {$$ = $2;}
            |   NUM                                             {$$ = $1;}
            |   VAR                                             {$$ = $1;}

%%

void display(struct tnode *node){
    if(node->nodetype == NULL)
        printf("NUM ");
    else{
        if(node->left != NULL)
            display(node->left);

        printf("%s ", node->nodetype);

        if(node->right != NULL)
            display(node->right);           
    }
}
void yyerror(char const *s)
{
    printf("yyerror  %s\n",s);
    return ;
}

int main(){
    
    yyparse();
    return 0;
}