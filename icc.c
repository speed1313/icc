#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Token kind
typedef enum{
    TK_RESERVED,//symbol
    TK_NUM,     //int Token
    TK_EOF,     //Token representing the end of the input
}TokenKind;

typedef struct Token Token;

//token type
struct Token{
    TokenKind kind; //token type
    Token *next;    //next input token
    int val;        //if kind is TK_NUM, its number
    char *str;       //token string
};

//Tokens we are currently focusing on
Token *token;

//function for reporting errors
//it takes the same arguments as printf
void error(char *fmt, ...){
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr,fmt,ap);
    fprintf(stderr,"\n");
    exit(1);
}

//when the next token is the expected symbol, read the token one more time
//and return true. Otherwise, return false.
bool consume(char op){
    if(token->kind != TK_RESERVED || token->str[0] != op)
        return false;
    token=token->next;
    return true;
}

//when the next token is the expected symbol, read the token one more time.
// Otherwise, report an error.
void expect(char op){
    if(token->kind!=TK_RESERVED||token->str[0]!=op)
        error("not '%c'",op);
    token=token->next;
}

// If the next token is a integer, read the token one more time and return the number.
// Otherwise, report an error.
int expect_number(){
    if(token->kind!=TK_NUM)
        error("not integer");
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

//Tokenize the input string p and return it.
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

        error("can't tokenize");
    }

    new_token(TK_EOF,cur,p);
    return head.next;
}

int main(int argc, char **argv){
    if(argc!=2){
        error("The number of arguments is incorrect");
        return 1;
    }

//tokenize
token=tokenize(argv[1]);

//output the first half of the assembly
printf(".intel_syntax noprefix\n");
printf(".globl main\n");
printf("main:\n");

//The first part of the expression must be a integer,
//so check for that and output the first mov instruction
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