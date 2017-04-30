%{
#include "syntax.tab.h"
#include "Node.h"
#define lexical_token_action(print_type, node_type, extra) \
{ \
    yylval.type_pointer = make_leaf(_##node_type, print_type, yylineno, extra); \
    return node_type; \
}
#define ignore_block_comment() \
{ \
    while (1) { \
        char c; \
        while ((c = input()) != '*' && c != EOF) { \
        } \
        if (c == EOF) break; \
        if ((c = input()) == '/' || c == EOF) break; \
        else unput(c); \
    } \
}
#define error_type_A(text) \
{ \
    fprintf(stderr, "Error type A at Line %d: Mysterious characters \'%s\'\n", yylineno, text); \
}


int yycolumn = 1;
#define YY_USER_ACTION \
    yylloc.first_line = yylloc.last_line = yylineno; \
    yylloc.first_column = yycolumn; \
    yylloc.last_column = yycolumn + yyleng - 1; \
    yycolumn += yyleng;
extern int err_occur;
%}
%option yylineno
DEC 0|([1-9][0-9]*)
OCT 0[0-7]+
HEX 0[xX][0-9a-fA-F]+
INT {DEC}|{OCT}|{HEX}
EXP ([eE][+-]?[0-9]+)
FLOAT ((([0-9]+\.[0-9]*)|(\.[0-9]+)){EXP}?)|(([0-9]+){EXP})
ID [_a-zA-Z][_a-zA-Z0-9]*
WS [ \t]+
NL \n
LCOMMENT \/\/.*\n
BCOMMENT \/\*
COMMENT {LCOMMENT}|{BCOMMENT}
SEMI ;
COMMA ,
ASSIGNOP =
RELOP [><]|([><=!]=)
PLUS \+
MINUS -
STAR \*
DIV \/
AND &&
OR \|\|
DOT \.
NOT !
TYPE int|float
LP \(
RP \)
LB \[
RB \]
LC \{
RC \}
STRUCT struct
RETURN return
IF if
ELSE else
WHILE while
%%
{STRUCT}    {lexical_token_action(LEXICAL_OTHER, STRUCT, NULL);}
{RETURN}    {lexical_token_action(LEXICAL_OTHER, RETURN, NULL);}
{IF}        {lexical_token_action(LEXICAL_OTHER, IF, NULL);}
{ELSE}      {lexical_token_action(LEXICAL_OTHER, ELSE, NULL);}
{WHILE}     {lexical_token_action(LEXICAL_OTHER, WHILE, NULL);}
{TYPE}      {lexical_token_action(LEXICAL_TYPE, TYPE, yytext);}
{INT}       {lexical_token_action(LEXICAL_INT, INT, yytext);}
{FLOAT}     {lexical_token_action(LEXICAL_FLOAT, FLOAT, yytext);}
{ID}        {lexical_token_action(LEXICAL_ID, ID, yytext);}
{LCOMMENT}  {}
{BCOMMENT}  {ignore_block_comment();}
{SEMI}      {lexical_token_action(LEXICAL_OTHER, SEMI, NULL);}
{COMMA}     {lexical_token_action(LEXICAL_OTHER, COMMA, NULL);}
{ASSIGNOP}  {lexical_token_action(LEXICAL_OTHER, ASSIGNOP, NULL);}
{RELOP}     {lexical_token_action(LEXICAL_OTHER, RELOP, NULL);}
{PLUS}      {lexical_token_action(LEXICAL_OTHER, PLUS, NULL);}
{MINUS}     {lexical_token_action(LEXICAL_OTHER, MINUS, NULL);}
{STAR}      {lexical_token_action(LEXICAL_OTHER, STAR, NULL);}
{DIV}       {lexical_token_action(LEXICAL_OTHER, DIV, NULL);}
{AND}       {lexical_token_action(LEXICAL_OTHER, AND, NULL);}
{OR}        {lexical_token_action(LEXICAL_OTHER, OR, NULL);}
{DOT}       {lexical_token_action(LEXICAL_OTHER, DOT, NULL);}
{NOT}       {lexical_token_action(LEXICAL_OTHER, NOT, NULL);}
{LP}        {lexical_token_action(LEXICAL_OTHER, LP, NULL);}
{RP}        {lexical_token_action(LEXICAL_OTHER, RP, NULL);}
{LB}        {lexical_token_action(LEXICAL_OTHER, LB, NULL);}
{RB}        {lexical_token_action(LEXICAL_OTHER, RB, NULL);}
{LC}        {lexical_token_action(LEXICAL_OTHER, LC, NULL);}
{RC}        {lexical_token_action(LEXICAL_OTHER, RC, NULL);}
{WS}        {}
{NL}        {yycolumn = 1;}
.           {err_occur = 1; error_type_A(yytext);}
%%
