#include "myscanner.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "strmap.h"

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
StrMap *symbol_Table;
char* stack[100];
char* pb[100];
int pc = 0;
int top = 0;
int breakPCStack[100][100];
int loopBoolStack[100];
int loopBreakCount[100];
int loopCount = 0;

int pushStack(char *toBePushed){
    printf("\nPushingStack: %s\n", toBePushed);
    char *newCopy = malloc((strlen(toBePushed) + 1) * sizeof(char));
    strcpy(newCopy, toBePushed);
    stack[top] = newCopy;
    top++;
    return 0;
}

char* popStack(){
    if(top > 0){
        return stack[--top];
    }
    else{
        return stack[top];
    }
}


int main() {
    symbol_Table = sm_new(100);
    nToken = yylex();
    block();
    printf("Parsing Successful!\n");
    for(int i = 0;i<=pc;i++){
        printf("%d) %s", i, pb[i]);
    }
    return 0;
}

char *getMemory(int numberOfBytes) {
    static int currentMem = 100;
    int returnValue = currentMem;
    currentMem += numberOfBytes;
    char *result = malloc(4 * sizeof(char));
    sprintf(result, "%d", currentMem);
    return result;
}

void addToPB(char *command){
    pb[pc++] = command;
}

void sub(char *first, char *second){
    char *temp = getMemory(1);
    pushStack(temp);
    char *command = malloc(20 * sizeof(char));
    sprintf(command, "(-, %s, %s, %s)\n", temp, first, second);
    addToPB(command);
}

void bool(char *operator){
    char *temp = getMemory(1);
    char *first = popStack();
    char *second = popStack();
    pushStack(temp);
    char *command = malloc(20 * sizeof(char));
    sprintf(command, "(%s, %s, %s, %s)\n", operator, temp, first, second);
    addToPB(command);
}

void add(){
    char *first = popStack();
    char *second = popStack();
    char *temp = getMemory(1);
    pushStack(temp);
    char *command = malloc(20 * sizeof(char));
    sprintf(command, "(+, %s, %s, %s)\n", temp, first, second);
    addToPB(command);
}

void multiply(){
    char *first = popStack();
    char *second = popStack();
    char *temp = getMemory(1);
    pushStack(temp);
    char *command = malloc(20 * sizeof(char));
    sprintf(command, "(*, %s, %s, %s)\n", temp, first, second);
    addToPB(command);
}

void devide(){
    char *second = popStack();
    char *first = popStack();
    char *temp = getMemory(1);
    pushStack(temp);
    char *command = malloc(20 * sizeof(char));
    printf(command, "(/, %s, %s, %s)\n", temp, first, second);
    addToPB(command);
}

void save(){
    char *current = malloc(3 * sizeof(char));
    sprintf(current, "%d", pc);
    pushStack(current);
    pc++;
}

void save_jmp(){
    char *code = malloc(20 * sizeof(char));
    int pbC = atoi(popStack());
    printf("PBC::::%d", pbC);
    sprintf(code, "(JUMPF, %s, , %d)\n", popStack(), pc + 1);
    pb[pbC] = code;
    char *secondCode = malloc(20 * sizeof(char));
    sprintf(secondCode, "(JMP, , %d)\n", pbC);
    pb[pc++] = secondCode;
}

void save_cjmp(int hasElse){
    char *code = malloc(20 * sizeof(char));
    // printf("popStack:%s\n", popStack());
    // printf("popStack:%s\n", popStack());
    int pbC = atoi(popStack());
    printf("SaveCJUMP:::PBC::::%d\n", pbC);
    if(hasElse == 0)
        sprintf(code, "(JUMPF, %s, , %d)\n", popStack(), pc);
    else sprintf(code, "(JUMPF, %s, , %d)\n", popStack(), pc + 1);
    pb[pbC] = code;
}

void cjmp(){
    char *code = malloc(20 * sizeof(char));
    sprintf(code, "(JMP, , , %d)\n", pc);
    int pbC = atoi(popStack());
    printf("CJUMP::::::%d\n", pbC);
    pb[pbC] = code;
}

char *add_symbol(char *lexeme) {
    if(sm_exists(symbol_Table, lexeme) == 1){
        printf("\nVariable is already declared: %s\n", lexeme);
        exit(1);
    }
    char *memoryNumber = getMemory(1);
    // char *memNumber = malloc(3 * sizeof(char));
    // strcpy(memNumber, "10");
    sm_put(symbol_Table, lexeme, memoryNumber);
    return memoryNumber;
}

void *minusSign(){
    char *shouldMinus = popStack();
    pushStack(shouldMinus);
    char *command = malloc(20 * sizeof(char));
    sprintf(command, "(-, %s)\n", shouldMinus);
    addToPB(command);
}

char *get_symbol(char *lexeme) {
    if(sm_exists(symbol_Table, lexeme) == 0){
        printf("\nVariable not declared: %s\n", lexeme);
        exit(1);
    }
    int numberOfBytesNeeded = sm_get(symbol_Table, lexeme, NULL, 0);
    if(numberOfBytesNeeded == 0){
        // printf("\nVariable not declared: %s\n", lexeme);
        // exit(1);
        return add_symbol(lexeme);
    }
    char *result = malloc(numberOfBytesNeeded);
    sm_get(symbol_Table, lexeme, result, numberOfBytesNeeded);
    return result;
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
        add();
        BPrim();
        return 0;
    }
    if(strcmp(yytext, "-") == 0){
        match("-");
        T();
        char *second = popStack();
        char *first = popStack();
        sub(first, second);
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
        multiply();
        PPrim();
        return 0;
    }
    if(strcmp(yytext, "/") == 0){
        match("/");
        P();
        devide();
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
        minusSign();
        return 0;
    }
    if(nToken == 4){
        pushStack(get_symbol(yytext));
        nToken = yylex();
        return 0;
    }
    if(nToken == 5){
        char *literal = malloc((strlen(yytext) + 3) * sizeof(char));
        sprintf(literal, "\"%s\"", yytext);
        pushStack(literal);
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
        printf("HELLO");
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
            bool("<");
            return 0;
        }
        if(strcmp(yytext, "<=") == 0){
            match("<=");
            operand();
            bool("<=");
            return 0;
        }
        if(strcmp(yytext, ">") == 0){
            match(">");
            operand();
            bool(">");
            return 0;
        }
        if(strcmp(yytext, ">=") == 0){
            match(">=");
            operand();
            bool(">=");
            return 0;
        }
        if(strcmp(yytext, "=") == 0){
            match("=");
            operand();
            bool("=");
            return 0;
        }
        if(strcmp(yytext, "<>") == 0){
            match("<>");
            operand();
            bool("<>");
            return 0;
        }
    }
    return 0;
}

int operand(){
    printf("\nOPERAND, IDTOKEN: %d\n", nToken);
    if(nToken == 1 || nToken == 5){
        char *literal = malloc((strlen(yytext) + 3) * sizeof(char));
        sprintf(literal, "\"%s\"", yytext);
        pushStack(literal);
        nToken = yylex();
        return 0;
    }
    if(nToken == 4){
        pushStack(get_symbol(yytext));
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
    if(strcmp(yytext, "break") == 0 && loopCount > 0){
        break_stmt();
        breakPCStack[loopCount - 1][loopBreakCount[loopCount - 1]++] = pc++;
        return 0;
    }
    if(strcmp(yytext, "continue") == 0 && loopCount > 0){
        continue_stmt();
        char *code = malloc(20 * sizeof(char));
        sprintf(code, "(JMP, , %d)\n", loopBoolStack[loopCount - 1]);
        addToPB(code);
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
    save();
    match("then");
    stmt(); 
    APrim();
    return 0;
}

int APrim() {
    if(strcmp(yytext, "else") == 0){
        save_cjmp(1);
        match("else");
        save();
        stmt();
        cjmp();
        return 0;
    }
    else save_cjmp(0);
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
    char *to;
    char *from;
    if(nToken == 4){
        pushStack(get_symbol(yytext));
        nToken = yylex();
    }
    match(":=");
    cexpr();
    from = popStack();
    to = popStack();
    char *command = malloc(20 *sizeof(char));
    sprintf(command, "(:=, %s, %s)\n", to, from);
    addToPB(command);
    return 0;
}

int while_stmt(){
    match("while");
    bexpr();
    loopBoolStack[loopCount] = pc;
    printf("PC: %d", pc);
    loopBreakCount[loopCount] = 0;
    save();
    loopCount++;
    match("do");
    stmt();
    save_jmp();
    loopCount--;
    for(int i = 0;i<loopBreakCount[loopCount];i++){
        char *code = malloc(20 * sizeof(char));
        sprintf(code, "(JMP, , %d)\n", pc);
        int pbC = breakPCStack[loopCount][i];
        printf("pbC: %d\n", pbC);
        pb[pbC] = code;
    }
    return 0;
}

int dcls(){
    if(
    strcmp("int", yytext) == 0 ||
    strcmp("bool", yytext) == 0 ||
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
    add_symbol(yytext);
    match("id");
    A();
    return 0;
}

int A(){
    if(strcmp(yytext, ",") == 0){
        match(",");
        if(nToken == 4){
            add_symbol(yytext);
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
    if( strcmp("bool", yytext) == 0){
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