#include "icc.h"

bool consume(char *op){
    if(token->kind != TK_RESERVED ||
            strlen(op)!=token->len||
            memcmp(token->str,op,token->len))
        return false;
    token=token->next;
    return true;
}
Token *consume_ident(){
    if(token->kind==TK_IDENT){
        Token *tok2=token;
        token=token->next;
        return tok2;
    }
    return 0;
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
        if('a'<=*p && *p<='z'){
            cur = new_token(TK_IDENT, cur, p++,1);
            continue;
        }
        if(strchr(";",*p)){
            cur=new_token(TK_RESERVED,cur,p++,1);
            continue;
        }
        if(strchr("=",*p)){
            cur=new_token(TK_RESERVED,cur,p++,1);
            continue;
        }

        error_at(p,"invalid token");
    }

    new_token(TK_EOF,cur,p,0);
    return head.next;
}
