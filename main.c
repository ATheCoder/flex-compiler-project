#include "myscanner.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int match(char *toMatch);
int cexpr();
int BPrim();
int T();
int PPrim();
int P();
int bexpr();
int QPrim();
int Q();
int rexpr();
int operand();
int step();
int block();
int stmts();
int stmt();
int print_stmt();
int read_stmt();
int if_stmt();
int APrim();
int continue_stmt();
int break_stmt();
int assign_stmt();
int while_stmt();
int dcls();
int dcl();
int id_list();
int A();
int type();
void throwError();

extern int yylex();
extern int yylineno;
extern char* yytext;
int nToken;

int main() {
    nToken = yylex();
    block();
    printf("Parsing Successful!");
    return 0;
}

int match(char* toMatch){
    if(strcmp(toMatch, "id") == 0 && nToken == 4){
        nToken = yylex();
        return 0;
    }
    if(strcmp(toMatch, yytext) == 0){
        nToken = yylex();
        return 0;
    }
    else{
        printf("Syntax Error: Unexpected Token: \"%s\" on line %d\n", yytext, yylineno);
        printf("Expected: %s", toMatch);
        exit(1);
    }
    return 0;
}

int cexpr(){
    T();
    BPrim();
    return 0;
}

int BPrim(){
    if(strcmp(yytext, "+") == 0){
        match("+");
        T();
        BPrim();
        return 0;
    }
    if(strcmp(yytext, "-") == 0){
        match("-");
        T();
        BPrim();
        return 0;
    }
    return 0;
}

int T(){
    P();
    PPrim();
    return 0;
}

int PPrim(){
    if(strcmp(yytext, "*") == 0){
        match("*");
        P();
        PPrim();
        return 0;
    }
    if(strcmp(yytext, "/") == 0){
        match("-");
        P();
        PPrim();
        return 0;
    }
    return 0;
}

int P(){
    if(strcmp(yytext, "(") == 0){
        match("(");
        cexpr();
        match(")");
        return 0;
    }
    if(strcmp(yytext, "-") == 0){
        match("-");
        cexpr();
        return 0;
    }
    if(nToken == 4){
        nToken = yylex();
        return 0;
    }
    if(nToken == 5){
        nToken = yylex();
        return 0;
    }
    throwError();
    return -1;
}

int bexpr(){
    Q();
    QPrim();
    return 0;
}

int QPrim(){
    if(strcmp(yytext, "or") == 0){
        match("or");
        Q();
        QPrim();
        return 0;
    }
    if(strcmp(yytext, "and") == 0){
        match("and");
        Q();
        QPrim();
        return 0;
    }
    return 0;
}

int Q(){
    if(strcmp(yytext, "not") == 0){
        match("not");
        bexpr();
        return 0;
    }
    if(strcmp(yytext, "(") == 0){
        match("(");
        bexpr();
        match(")");
        return 0;
    }
    rexpr();
    return 0;
}

int rexpr(){
    if(nToken == 4 || nToken == 5 || nToken == 1){
        operand();
        if(strcmp(yytext, "<") == 0){
            match("<");
            operand();
            return 0;
        }
        if(strcmp(yytext, "<=") == 0){
            match("<=");
            operand();
            return 0;
        }
        if(strcmp(yytext, ">") == 0){
            match(">");
            operand();
            return 0;
        }
        if(strcmp(yytext, ">=") == 0){
            match(">=");
            operand();
            return 0;
        }
        if(strcmp(yytext, "=") == 0){
            match("=");
            operand();
            return 0;
        }
        if(strcmp(yytext, "<>") == 0){
            match("<>");
            operand();
            return 0;
        }
    }
    operand();
    return 0;
}

int operand(){
    if(nToken == 4 || nToken == 5 || nToken == 1){
        nToken = yylex();
        return 0;
    }
    throwError();
    return -1;
}

int step(){
    if(strcmp(yytext, "by") == 0){
        match("by");
        cexpr();
        return 0;
    }
    return 0;
}

int block(){
    match("{");
    dcls();
    stmts();
    match("}");
    return 0;
}

int stmts() {
    if(
        nToken == 4 ||
        strcmp(yytext, "for") == 0 ||
        strcmp(yytext, "while") == 0 ||
        strcmp(yytext, "{") == 0 ||
        strcmp(yytext, "break") == 0 ||
        strcmp(yytext, "continue") == 0 ||
        strcmp(yytext, "if") == 0 ||
        strcmp(yytext, "read") == 0 ||
        strcmp(yytext, "print") == 0
    ){
        stmt();
        stmts();
        return 0;
    }
    return 0;
}

int stmt() {
    if(nToken == 4){
        assign_stmt();
        match(";");
        return 0;
    }
    if(strcmp(yytext, "for") == 0){
        match("for");
        if(nToken == 4){
            nToken = yylex();
        }
        match(":=");
        cexpr();
        match("to");
        cexpr();
        step();
        match("do");
        stmt();
        return 0;
    }
    if(strcmp(yytext, "while") == 0){
        while_stmt();
        return 0;
    }
    if(strcmp(yytext, "{") == 0){
        block();
        return 0;
    }
    if(strcmp(yytext, "break") == 0){
        break_stmt();
        return 0;
    }
    if(strcmp(yytext, "continue") == 0){
        continue_stmt();
        return 0;
    }
    if(strcmp(yytext, "if") == 0){
        if_stmt();
        return 0;
    }
    if(strcmp(yytext, "read") == 0){
        read_stmt();
        return 0;
    }
    if(strcmp(yytext, "print") == 0){
        print_stmt();
        return 0;
    }
    throwError();
    return -1;

}

int print_stmt(){
    match("print");
    match("(");
    id_list();
    match(")");
    match(";");
    return 0;
}

int read_stmt(){
    match("read");
    match("(");
    id_list();
    match(")");
    match(";");
    return 0;
}

int if_stmt() {
    match("if");
    bexpr();
    match("then");
    stmt();
    APrim();
    return 0;
}

int APrim() {
    printf("%s\n", yytext);
    if(strcmp(yytext, "else") == 0){
        match("else");
        stmt();
        return 0;
    }
    return 0;
}

int continue_stmt(){
    match("continue");
    match(";");
    return 0;
}

int break_stmt(){
    match("break");
    match(";");
    return 0;
}

int assign_stmt(){
    if(nToken == 4){
        nToken = yylex();
    }
    match(":=");
    cexpr();
    return 0;
}

int while_stmt(){
    match("while");
    bexpr();
    match("do");
    stmt();
    return 0;
}

int dcls(){
    if(
    strcmp("int", yytext) == 0 ||
    strcmp("float", yytext) == 0 || 
    strcmp("char", yytext) == 0 ||
    strcmp("long", yytext) == 0 ||
    strcmp("double", yytext) == 0 ){
        dcl();
        dcls();
        return 0;
    }
    return 0;
}

int dcl(){
    type();
    id_list();
    match(";");
    return 0;
}
int id_list(){
    match("id");
    A();
    return 0;
}

int A(){
    if(strcmp(yytext, ",") == 0){
        match(",");
        if(nToken == 4){
            nToken = yylex();
            A();
            return 0;
        }
    }
    return 0;
}

int type(){
    if( strcmp("int", yytext) == 0){
        nToken = yylex();
        return 0;
    }
    if( strcmp("float", yytext) == 0){
        nToken = yylex();
        return 0;
    }
    if( strcmp("char", yytext) == 0){
        nToken = yylex();
        return 0;
    }
    if( strcmp("long", yytext) == 0){
        nToken = yylex();
        return 0;
    }
    if( strcmp("double", yytext) == 0 ){
        nToken = yylex();
        return 0;
    }
    throwError();
    return -1;
}

void throwError() {
    printf("Syntax Error: Unexpected Token: \"%s\" on line %d\n", yytext, yylineno);
    exit(1);
}