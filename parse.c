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
// 変数を名前で検索する。見つからなかった場合はNULLを返す。
LVar *find_lvar(Token *tok) {
    LVar *var=locals;
  for (var ; var; var = var->next){
    if (var->len == tok->len && strncmp(tok->str, var->name, var->len)==0){
      return var;
  }
  }
  return NULL;
}
int is_alnum(char c) {
  return ('a' <= c && c <= 'z') ||
         ('A' <= c && c <= 'Z') ||
         ('0' <= c && c <= '9') ||
         (c == '_');
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
        if(startswith(p,"==")||startswith(p,"!=")||
            startswith(p,"<=")||startswith(p,">=")){
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
            cur->val = strtol(p,&p,10);
            cur->len = p-q;
            continue;
        }
        if (strncmp(p, "return", 6) == 0 && !is_alnum(p[6])) {
            cur=new_token(TK_RETURN,cur,p,6);
            p += 6;
            continue;
}
        if('a'<=*p && *p<='z'||'A' <= *p && *p <= 'Z'){
            int i=0;
            char *q=p;
            i++;
            p++;
            while(('a' <= *p && *p <= 'z'||'A' <= *p && *p <= 'Z'||
            '0' <= *p && *p <= '9'||*p == '_') && i<30 ){
                i++;
                p++;
            }
            cur = new_token(TK_IDENT, cur, q, i);
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
