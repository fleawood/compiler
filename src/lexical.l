%{
#include "syntax.tab.h"
#include "Pointer.h"
#define lexical_token_action(node_type, name, extra) \
{ \
    yylval.type_pointer = make_node(node_type, #name, yylineno, extra); \
    return name; \
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
{STRUCT}    {lexical_token_action(LEXICAL_OTHER_NODE, STRUCT, NULL);}
{RETURN}    {lexical_token_action(LEXICAL_OTHER_NODE, RETURN, NULL);}
{IF}        {lexical_token_action(LEXICAL_OTHER_NODE, IF, NULL);}
{ELSE}      {lexical_token_action(LEXICAL_OTHER_NODE, ELSE, NULL);}
{WHILE}     {lexical_token_action(LEXICAL_OTHER_NODE, WHILE, NULL);}
{TYPE}      {lexical_token_action(LEXICAL_TYPE_NODE, TYPE, yytext);}
{INT}       {lexical_token_action(LEXICAL_INT_NODE, INT, yytext);}
{FLOAT}     {lexical_token_action(LEXICAL_FLOAT_NODE, FLOAT, yytext);}
{ID}        {lexical_token_action(LEXICAL_ID_NODE, ID, yytext);}
{LCOMMENT}  {}
{BCOMMENT}  {ignore_block_comment();}
{SEMI}      {lexical_token_action(LEXICAL_OTHER_NODE, SEMI, NULL);}
{COMMA}     {lexical_token_action(LEXICAL_OTHER_NODE, COMMA, NULL);}
{ASSIGNOP}  {lexical_token_action(LEXICAL_OTHER_NODE, ASSIGNOP, NULL);}
{RELOP}     {lexical_token_action(LEXICAL_OTHER_NODE, RELOP, NULL);}
{PLUS}      {lexical_token_action(LEXICAL_OTHER_NODE, PLUS, NULL);}
{MINUS}     {lexical_token_action(LEXICAL_OTHER_NODE, MINUS, NULL);}
{STAR}      {lexical_token_action(LEXICAL_OTHER_NODE, STAR, NULL);}
{DIV}       {lexical_token_action(LEXICAL_OTHER_NODE, DIV, NULL);}
{AND}       {lexical_token_action(LEXICAL_OTHER_NODE, AND, NULL);}
{OR}        {lexical_token_action(LEXICAL_OTHER_NODE, OR, NULL);}
{DOT}       {lexical_token_action(LEXICAL_OTHER_NODE, DOT, NULL);}
{NOT}       {lexical_token_action(LEXICAL_OTHER_NODE, NOT, NULL);}
{LP}        {lexical_token_action(LEXICAL_OTHER_NODE, LP, NULL);}
{RP}        {lexical_token_action(LEXICAL_OTHER_NODE, RP, NULL);}
{LB}        {lexical_token_action(LEXICAL_OTHER_NODE, LB, NULL);}
{RB}        {lexical_token_action(LEXICAL_OTHER_NODE, RB, NULL);}
{LC}        {lexical_token_action(LEXICAL_OTHER_NODE, LC, NULL);}
{RC}        {lexical_token_action(LEXICAL_OTHER_NODE, RC, NULL);}
{WS}        {}
{NL}        {yycolumn = 1;}
. {err_occur = 1; printf("Error type A at Line %d: Mysterious characters \'%s\'\n", yylineno, yytext);}
%%
