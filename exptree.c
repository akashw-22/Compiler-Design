#include <stdlib.h>
#include <string.h>

#define OUTPUT "B190532CS.xsm"

int h_reg = -1;
int label = 0;
FILE *fp;
FILE *fp2;
struct Gsymbol* head = NULL;

struct Gsymbol* createst(char *c, int binding){
    
    struct Gsymbol *temp;
    temp = (struct Gsymbol*)malloc(sizeof(struct Gsymbol));
    temp->name = malloc(sizeof(char)*strlen(c));
    strcpy(temp->name, c);
    printf("creating st name: %s\n", temp->name);
    temp->binding = binding;
    temp->next = NULL;
}

int searchst(struct Gsymbol* entry, char *c){
    
    if(entry == NULL)
        return -1;
    else{

        if(!strcmp(entry->name, c))
            return entry->binding;
        
        else
            return searchst(entry->next, c);
    }

}

int getBinding(char *c){
    
    int pos;

    if(head == NULL){
        head = createst(c, 0);
        return 0;
    }

    pos = searchst(head, c);

    if(pos == -1){
        struct Gsymbol* temp;
        temp = head;
        head = createst(c, temp->binding+1);
        head->next = temp;
        pos = head->binding;
    }

    return pos;
}

struct tnode* makeLeaf(int n){
    
    struct tnode *node;
    
    node = (struct tnode*)malloc(sizeof(struct tnode));
    node->val = n;
    node->right = NULL;
    node->left = NULL;
    node->nodetype = NULL;

    return node;
}

struct tnode* makeID(char *c){
    
    struct tnode *node;
    
    node = (struct tnode*)malloc(sizeof(struct tnode));
    node->nodetype = malloc(sizeof(char));
    *(node->nodetype) = 'i';
    node->varname = malloc(strlen(c)*sizeof(char));
    strcpy(node->varname, c);
    node->left = NULL;
    node->right = NULL;

    return node;
}

struct tnode* makeInternal(char c, struct tnode *left, struct tnode *right){

    struct tnode *node;
    
    node = (struct tnode*)malloc(sizeof(struct tnode));
    node->nodetype = malloc(sizeof(char));
    *(node->nodetype) = c;
    node->left = left;
    node->right = right;
    
    return node;
}

struct tnode* makeIf(struct tnode *exp, struct tnode *ifnode, struct tnode *elsenode){

    struct tnode *node, *conn;
    
    conn = makeInternal('c', ifnode, elsenode);
    node = makeInternal('f', exp, conn);
    
    return node;
}

int evaluate(struct tnode* node){
    
    if(node->nodetype == NULL){
        return node->val;
    }

    else{
        switch(*(node->nodetype)){
            
            case '+': return evaluate(node->left) + evaluate(node->right);
                        break;
            case '*': return evaluate(node->left) * evaluate(node->right);
                        break;
        }
    }                   
}

void createfile(struct tnode *node){

    //printf("test\n");
    //printf("\n\ncreating files..\n\n");
    
    //fp = fopen("../xsm_expl/test.xsm", "w");

    fp = fopen("labeled.xsm", "w");
    
    fprintf(fp, "%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n",0,2056,0,0,0,0,0,0);
    fprintf(fp, "BRKP\n");   
    printf("Back to Register:%d\n", codegen(node));
    
    fprintf(fp, "MOV SP, 4116\nMOV R0, 10\nPUSH R0\n"
                    "PUSH R0\nPUSH R0\nPUSH R0\nPUSH R0\nINT 10\n");
        
    fclose(fp);
    
    unlabeled();
}

int codegen(struct tnode *node){

    int t1, t2, t3, t4;

    //i: ID, r: read, w: write, g: >=, l: <=, e: ==, !: !=, c: connector, f: if, d: dowhile, NULL: NUM

    if(node->nodetype == NULL){
        h_reg++;
        fprintf(fp, "MOV R%d, %d\n", h_reg, node->val);
        return h_reg;

    }
    else if(*(node->nodetype) == 'f'){
        //printf("test\n");
        
        t1 = codegen(node->left);
        
        int elselabel = ++label;
        
        fprintf(fp, "JZ R%d, L %d\n", t1, elselabel);
        h_reg--;
        

        //printf("h_reg(beforeif):%d ", h_reg);
        t2 = codegen(node->right->left);
        //printf("h_reg(afterif):%d\n", h_reg);

        int outlabel = ++label;
        fprintf(fp, "JMP L %d\n", outlabel);
        fprintf(fp, "Label %d:\n", elselabel);
        
       
        if(node->right->right != NULL){
            t3 = codegen(node->right->right);
            
        }
       
        fprintf(fp, "Label %d:\n", outlabel);
        return t3;

    }
    else if(*(node->nodetype) == 'd'){
        int dolabel = ++label;
        
        fprintf(fp, "Label %d:\n", dolabel);
        
        t1 = codegen(node->right);
        t2 = codegen(node->left);
        
        fprintf(fp, "JZ R%d, L %d\n", t2, ++label);
        h_reg--;

        fprintf(fp, "JMP L %d\n", dolabel);
        fprintf(fp, "Label %d:\n", label);
        
        return h_reg;
        
    }
    else if(*(node->nodetype) == 'i'){
       
        int binding = searchst(head, node->varname);
        
        if(binding == -1){
            printf("Variable not found in symboltable\n");
            return -1;
        }
        h_reg++;
        fprintf(fp, "MOV R%d, [%d]\n", h_reg, 4096 + binding);
        return h_reg;
    }
    else if(*(node->nodetype) == 'r'){
        
        int binding = getBinding(node->left->varname);
        //printf("read-binding: %d\n", binding);

        h_reg += 2;
        //printf("Reading - h_reg: %d\n", h_reg);
        
        fprintf(fp, "MOV SP, 4116\nMOV R%d, 7\nPUSH R%d\nMOV R%d, -1\n"
                    "PUSH R%d\nMOV R%d, %d\nPUSH R%d\nPUSH R%d\nPUSH R%d\nINT 6\n", h_reg, h_reg, h_reg, h_reg, h_reg, 4096 + binding, h_reg, h_reg, h_reg);
        
        fprintf(fp, "POP R%d\nPOP R%d\nPOP R%d\nPOP R%d\nPOP R%d\n", h_reg, h_reg, h_reg-1, h_reg, h_reg);
        
        h_reg--;
        
        //printf("varname: %c, stack position to enter: %d\n", *(node->left->varname), (int)*(node->left->varname) - 'a');
        
        //fprintf(fp, "MOV SP, %d\n", 4096 + ((int)*(node->left->varname) - 'a'));
        //fprintf(fp, "MOV [SP], [R%d]\n", h_reg);
        h_reg--;
        return h_reg;
    }
    else if(*(node->nodetype) == 'w')
    {   
        t1 = codegen(node->left);
        h_reg += 2;
        //printf("h_reg: %d\n", h_reg);
        
        fprintf(fp, "MOV SP, 4116\nMOV R%d, 5\nPUSH R%d\nMOV R%d, -2\n"
                    "PUSH R%d\nPUSH R%d\nPUSH R%d\nPUSH R%d\nINT 7\n", h_reg, h_reg, h_reg, h_reg, t1, h_reg, h_reg);
        
        fprintf(fp, "POP R%d\nPOP R%d\nPOP R%d\nPOP R%d\nPOP R%d\n", h_reg, h_reg, h_reg-1, h_reg, h_reg);
        
        h_reg -= 3;
        return h_reg;
    }
   
    else if(*(node->nodetype) == '='){
        
        int binding = getBinding(node->left->varname);
        printf("read-binding: %d\n", binding);

        t1 = codegen(node->right);

        
        fprintf(fp, "MOV SP, %d\n", 4096 + binding);
        fprintf(fp, "MOV [SP], R%d\n", t1);
        //printf("current hg: %d, t1: %d\n", h_reg, t1);
        
        h_reg--;
        
        return h_reg;
    }
    else if(*(node->nodetype) == 'c')
    {
        t1 = codegen(node->left);
        t2 = codegen(node->right);
        
        return h_reg;
    }
    else{


        t1 = codegen(node->left);
        t2 = codegen(node->right);

        if(t1 < t2)
            t3 = t1;
        else
            t3 = t2;
        h_reg--;

        //printf("Register for +, * t1: %d, t2: %d, h_reg: %d\n",t1,t2,h_reg);

        switch(*(node->nodetype)){
            
            case '+':   fprintf(fp, "ADD R%d, R%d\n", t3, t1 + t2 - t3);
                        return t3;
                        break;

            case '*':   fprintf(fp, "MUL R%d, R%d\n", t3, t1 + t2 - t3);
                        return t3;
                        break;

            case '>':   fprintf(fp, "GT R%d, R%d\n", t3, t1 + t2 - t3);
                        return t3;
                        break;

            case '<':   fprintf(fp, "LT R%d, R%d\n", t3, t1 + t2 - t3);
                        return t3;
                        break;

            case 'e':   fprintf(fp, "EQ R%d, R%d\n", t3, t1 + t2 - t3);
                        return t3;
                        break;

            case '!':   fprintf(fp, "NE R%d, R%d\n", t3, t1 + t2 - t3);
                        return t3;
                        break;

            case 'g':   fprintf(fp, "GE R%d, R%d\n", t3, t1 + t2 - t3);
                        return t3;
                        break;

            case 'l':   fprintf(fp, "LE R%d, R%d\n", t3, t1 + t2 - t3);
                        return t3;
                        break;
        }
    }

}

void unlabeled(){
    
    // fp = fopen("../xsm_expl/test.xsm", "r");
    // fp2 = fopen("../xsm_expl/unlabeled.xsm", "w");

    fp = fopen("labeled.xsm", "r");
    fp2 = fopen(OUTPUT, "w");
    printf("test");

    int arr[100], pos = 0;
    char buff[100], temp[50];
    char *token;
    int c_line = 0, lno;

    for(int i = 0; i < 100; i++)
        arr[i] = 0;
        
    
    while(fgets(buff, 100, fp) != NULL){
        
        //printf("%s\n",buff);
        strcpy(temp, buff);
        token = strtok(temp, " ");
        c_line++;

        if(!strcmp(token, "Label")){
            token = strtok(NULL, ":");
            lno = atoi(token);
            arr[lno] = ((c_line - (pos)) - 9)*2 + 2056;
            //printf("Buffer:%s, lineno: %d, arr[l%d]: %d\n", buff, c_line, lno, arr[lno]);
            pos++;
        }
    }
    
    fseek(fp, 0, SEEK_SET);

    while(fgets(buff, 100, fp) != NULL){
        
        strcpy(temp, buff);
        token = strtok(temp, " ");
        
        if(!strcmp(token, "Label"))
            continue;
        else if(!strcmp(token, "JMP")){
            token = strtok(NULL, " ");
            token = strtok(NULL, " ");
            lno = atoi(token);
            sprintf(buff, "JMP %d\n", arr[lno]);
        }
        else if(!strcmp(token, "JZ")){
            token = strtok(NULL, " ");
            sprintf(temp, "JZ %s", token);
            token = strtok(NULL, " ");
            token = strtok(NULL, " ");
            lno = atoi(token);
            sprintf(buff, "%s %d\n", temp, arr[lno]);
        }
        
        fprintf(fp2, "%s", buff);
    }
}