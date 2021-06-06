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
};


//Input program
char *user_input;

//Current token
Token *token;



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
bool consume(char op){
    if(token->kind != TK_RESERVED || token->str[0] != op)
        return false;
    token=token->next;
    return true;
}

//Ensure that the current token is 'op'
void expect(char op){
    if(token->kind!=TK_RESERVED||token->str[0]!=op)
        error_at(token->str,"not '%c'",op);
    token=token->next;
}

//Ensure that the current token is TK_NUM.
int expect_number(){
    if(token->kind!=TK_NUM)
        error_at(token->str,"not integer");
    int val=token->val;
    token=token->next;
    return val;
}

bool at_eof(){
    return token->kind==TK_EOF;
}

//create a new token and connect it to cur
Token *new_token(TokenKind kind,Token *cur,char *str){
    Token *tok=calloc(1,sizeof(Token));
    tok->kind=kind;
    tok->str=str;
    cur->next=tok;
    return tok;
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
        if(*p=='+'||*p=='-'){
            cur=new_token(TK_RESERVED,cur,p++);
            continue;
        }
        if(isdigit(*p)){
            cur=new_token(TK_NUM,cur,p);
            cur->val=strtol(p,&p,10);
            continue;
        }

        error_at(p,"can't tokenize");
    }

    new_token(TK_EOF,cur,p);
    return head.next;
}

int main(int argc, char **argv){
    if(argc!=2){
        error_at(token->str,"The number of arguments is incorrect");
        return 1;
    }

user_input=argv[1];

//tokenize
token=tokenize(argv[1]);

printf(".intel_syntax noprefix\n");
printf(".globl main\n");
printf("main:\n");

//The first part of the expression must be a integer.
printf("   mov rax, %d\n",expect_number());

//while consuming the sequence of tokens '+<number>' or
//'-<number>', Output the assembly

while(!at_eof()){
    if(consume('+')){
        printf("   add rax, %d\n", expect_number());
        continue;
    }
    expect('-');
    printf("   sub rax, %d\n", expect_number());
}

printf("   ret\n");
return 0;
}