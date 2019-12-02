%{
#define KEYWORD 0
#define LOGICAL_VALUE 1
#define LOGICAL_CMP 2
#define TYPE_DEF 3
#define IDENTIFIER 4
#define NUMBER 5
#define DELIMETER 6
#define OPERATOR 7
#define CMP_OPERATOR 8
#define BRACKETS 9
%}

delim           [;(),]
brackets        (\{|\})
ws              [\n\t]+
letter          [A-Za-z]
digit           [0-9]
id              {letter}({letter}|{digit})*
num             {digit}+(\.{digit}+)?
keyword         (if|then|else|by|long|for|to|do|while|repeat|until|break|continue|read|print)
logical_cmp     (and|or)
logical_value   (true|false)
type_def        (int|float|char|double)

%%

{ws}                ;
{logical_value}     return(LOGICAL_VALUE);
{logical_cmp}       return(LOGICAL_CMP);
{keyword}           return(KEYWORD);
{type_def}          return(TYPE_DEF);
{delim}             return(DELIMETER);
"+"                 return(OPERATOR);
"-"                 return(OPERATOR);
"*"                 return(OPERATOR);
"/"                 return(OPERATOR);
":="                return(OPERATOR);
"<"                 return(CMP_OPERATOR);
"<="                return(CMP_OPERATOR);
">"                 return(CMP_OPERATOR);
">="                return(CMP_OPERATOR);
"="                 return(CMP_OPERATOR);
"<>"                return(CMP_OPERATOR);
{brackets}          return(BRACKETS);
{num}               return(NUMBER);
{id}                return(IDENTIFIER);

%%

int yywrap(void) {
    return 1;
}