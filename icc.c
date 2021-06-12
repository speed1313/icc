#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Token kind
typedef enum{
    TK_RESERVED,//Keywords or punctuators
    TK_NUM,     //integer literals
    TK_EOF,     //End-of-file markers
}TokenKind;

typedef struct Token Token;

//token type
struct Token{
    TokenKind kind; //Token kind
    Token *next;    //next token
    int val;        //if kind is TK_NUM, its value
    char *str;       //Token string
    int len;        //Token length
};


//Input program
char *user_input;

//Current token
Token *token;

// Reports an error and exit.
void error(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

//report an error location and exit.
void error_at(char *loc, char *fmt, ...){
    va_list ap;
    va_start(ap,fmt);

    int pos=loc-user_input;
    fprintf(stderr,"%s\n",user_input);
    fprintf(stderr, "%*s",pos," ");//print pos whitespaces
    fprintf(stderr,"^ ");
    vfprintf(stderr,fmt,ap);
    fprintf(stderr,"\n");
    exit(1);

}

//consumes the current token if it matches 'op'
bool consume(char *op){
    if(token->kind != TK_RESERVED ||
            strlen(op)!=token->len||
            memcmp(token->str,op,token->len))
        return false;
    token=token->next;
    return true;
}

//Ensure that the current token is 'op'
void expect(char *op){
    if(token->kind!=TK_RESERVED||
            strlen(op)!=token->len||
            memcmp(token->str,op,token->len))
        error_at(token->str,"expected '%c'",op);
    token=token->next;
}

//Ensure that the current token is TK_NUM.
int expect_number(){
    if(token->kind!=TK_NUM)
        error_at(token->str,"expected a number");
    int val=token->val;
    token=token->next;
    return val;
}

bool at_eof(){
    return token->kind==TK_EOF;
}

//create a new token and connect it to cur
Token *new_token(TokenKind kind,Token *cur,char *str, int len){
    Token *tok=calloc(1,sizeof(Token));
    tok->kind=kind;
    tok->str=str;
    tok->len=len;
    cur->next=tok;
    return tok;
}

bool startswith(char *p, char *q){
    return memcmp(p,q,strlen(q))==0;
}
//Tokenize 'p' and returns new tokens.
Token *tokenize(char *p){
    Token head;
    head.next=NULL;
    Token *cur = &head;

    while(*p){
        //skip whitespace characters
        if(isspace(*p)){
            p++;
            continue;
        }
        if(startswith(p,"==")||startswith(p,"|=")||
            startswith(p,"<==")||startswith(p,">=")){
                cur=new_token(TK_RESERVED,cur,p,2);
                p+=2;
                continue;
        }
        if(strchr("+-*/()<>",*p)){
            cur=new_token(TK_RESERVED,cur,p++,1);
            continue;
        }

        if(isdigit(*p)){
            cur=new_token(TK_NUM,cur,p,0);
            char *q=p;
            cur->val=strtol(p,&p,10);
            cur->len=p-q;
            continue;
        }

        error_at(p,"invalid token");
    }

    new_token(TK_EOF,cur,p,0);
    return head.next;
}

typedef enum{
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_EQ,
    ND_NE,
    ND_LT,
    ND_LE,
    ND_NUM,
}NodeKind;

typedef struct Node Node;
struct Node{
    NodeKind kind;
    Node *lhs;
    Node *rhs;
    int val;
};

Node *new_node(NodeKind kind, Node *lhs, Node *rhs){
    Node *node=calloc(1,sizeof(Node));
    node->kind=kind;
    node->lhs=lhs;
    node->rhs=rhs;
    return node;
}
Node *new_node_num(int val){
    Node *node=calloc(1,sizeof(Node));
    node->kind=ND_NUM;
    node->val=val;
    return node;
}
Node *expr();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();

Node *expr(){
    return equality();
}
Node *equality(){
    Node *node=relational();

    for(;;){
        if(consume("=="))
            node=new_node(ND_EQ,node,relational());
        else if(consume("!="))
            node=new_node(ND_NE,node,relational());
        else
            return node;
    }
}
Node *relational(){
    Node *node=add();

    for(;;){
        if(consume("<"))
            node= new_node(ND_LT,node,add());
        else if(consume("<="))
            node=new_node(ND_LE,node,add());
        else if(consume(">"))
            node=new_node(ND_LT,add(),node);
        else if(consume(">="))
            node=new_node(ND_LE,add(),node);
        else
            return node;
    }
}


Node *add(){
    Node *node=mul();

    for(;;){
        if(consume("+"))
            node=new_node(ND_ADD,node,mul());
        else if(consume("-"))
            node=new_node(ND_SUB,node,mul());
        else
            return node;
    }
}
Node *mul(){
    Node *node=unary();
    for(;;){
        if(consume("*"))
            node=new_node(ND_MUL,node,unary());
        else if (consume("/"))
            node=new_node(ND_DIV,node,unary());
        else
            return node;
    }
}
Node *unary(){
    if(consume("+"))
        return unary();
    if(consume("-"))
        return new_node(ND_SUB,new_node_num(0),unary());
    return primary();
}
Node *primary(){
    if(consume("(")){
        Node *node=expr();
        expect(")");
        return node;
    }
    return new_node_num(expect_number());
}
void gen(Node *node){
    if(node->kind==ND_NUM){
        printf("    push %d\n",node->val);
        return;
    }
    gen(node->lhs);
    gen(node->rhs);
    printf("    pop rdi\n");
    printf("    pop rax\n");

    switch(node->kind){
        case ND_ADD:
            printf("    add rax, rdi\n");
            break;
        case ND_SUB:
            printf("    sub rax, rdi\n");
            break;
        case ND_MUL:
            printf("    imul rax, rdi\n");
            break;
        case ND_DIV:
            printf("    cqo\n");
            printf("    idiv rdi\n");
            break;
        case ND_EQ:
            printf("    cmp rax, rdi\n");
            printf("    sete al\n");
            printf("    movzb rax, al\n");
            break;
        case ND_NE:
            printf("    cmp rax, rdi\n");
            printf("    setne al\n");
            printf("    movzb rax, al\n");
            break;
        case ND_LT:
            printf("    cmp rax, rdi\n");
            printf("    setl al\n");
            printf("    movzb rax, al\n");
            break;
        case ND_LE:
            printf("    cmp rax, rdi\n");
            printf("    setle al\n");
            printf("    movzb rax, al\n");
            break;
    }
    printf("    push rax\n");
}
int main(int argc, char **argv){
    if(argc!=2){
        error_at(token->str,"The number of arguments is incorrect");
        return 1;
    }
    //tokenize and parse
    user_input=argv[1];
    token=tokenize(user_input);
    Node *node=expr();

    //print the first half of assembly
    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");

    gen(node);

    printf("    pop rax\n");
    printf("    ret\n");
    return 0;
}