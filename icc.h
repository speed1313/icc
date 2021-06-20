#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//Token kind
typedef enum{
    TK_RESERVED,//Keywords or punctuators
    TK_IDENT,   //identifer
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
typedef enum{// 演算子
    ND_ADD,     //+
    ND_SUB,     //-
    ND_MUL,     //*
    ND_DIV,     // /
    ND_EQ,      // =
    ND_NE,      //!=
    ND_LVAR,    //rocal arg
    ND_LT,      //<
    ND_LE,      //<=
    ND_NUM,     // integer
    ND_ASSIGN  // =
}NodeKind;

typedef struct LVar LVar;
//ローカル変数の型;
struct LVar{
    LVar *next; //　next LVar or NULL
    char name[128]; //name
    int len;    //length of name
    int offset; //offset from RBP
};

extern LVar *locals;    // local variable
extern char *user_input;
extern Token *token;

struct Node{
    NodeKind kind;
    struct Node *lhs;
    struct Node *rhs;
    int val;        //if only use kind is ND_NUM
    int offset;     //if only use kind is ND_LVAR
};
typedef struct Node Node;

bool consume(char *op);
Token *consume_ident();
void expect(char *op);
int expect_number();
bool at_eof();
void gen_lval(Node *node);
void gen(Node *node);
Token *tokenize(char *p);
Node *expr();
Node *program();
void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);
LVar *find_lvar(Token *tok);