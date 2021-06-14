#include "icc.h"
char *user_input;
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