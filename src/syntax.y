%{
#include "lex.yy.c"
#include "Node.h"
#include "token.h"
void yyerror(const char *msg);
extern Node *root;
#define syntax_token_action(p, node_type, lineno, n, ...) \
p = make_node(_##node_type, SYNTAX_TOKEN, lineno, NULL, n, __VA_ARGS__);
%}
%define parse.error verbose
%locations
%union {
    struct Node *type_pointer;
}
%token <type_pointer> TYPE
%token <type_pointer> INT FLOAT ID
%token <type_pointer> SEMI COMMA
%token <type_pointer> AND OR
%token <type_pointer> ASSIGNOP RELOP DOT NOT
%token <type_pointer> PLUS MINUS STAR DIV
%token <type_pointer> LP RP LB RB LC RC
%token <type_pointer> STRUCT RETURN IF ELSE WHILE

%type <type_pointer> Program ExtDefList ExtDef ExtDecList
%type <type_pointer> Specifier StructSpecifier OptTag Tag
%type <type_pointer> VarDec FunDec VarList ParamDec
%type <type_pointer> CompSt StmtList Stmt
%type <type_pointer> DefList Def DecList Dec
%type <type_pointer> Exp Args
%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right NOT
%left LP RP LB RB DOT
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE
%%
Program: ExtDefList {root = syntax_token_action($$, Program, @$.first_line, 1, $1);}
       ;
ExtDefList: ExtDef ExtDefList {syntax_token_action($$, ExtDefList, @$.first_line, 2, $1, $2);}
          | {$$ = NULL;}
          ;
ExtDef: Specifier ExtDecList SEMI {syntax_token_action($$, ExtDef, @$.first_line, 3, $1, $2, $3);}
      | Specifier SEMI {syntax_token_action($$, ExtDef, @$.first_line, 2, $1, $2);}
      | Specifier FunDec CompSt {syntax_token_action($$, ExtDef, @$.first_line, 3, $1, $2, $3);}
      | Specifier FunDec SEMI {syntax_token_action($$, ExtDef, @$.first_line, 3, $1, $2, $3);}
      | Specifier error SEMI {err_occur = 1;}
      ;
ExtDecList: VarDec {syntax_token_action($$, ExtDecList, @$.first_line, 1, $1);}
          | VarDec COMMA ExtDecList {syntax_token_action($$, ExtDecList, @$.first_line, 3, $1, $2, $3);}
          ;

Specifier: TYPE {syntax_token_action($$, Specifier, @$.first_line, 1, $1);}
         | StructSpecifier {syntax_token_action($$, Specifier, @$.first_line, 1, $1);}
         ;
StructSpecifier: STRUCT OptTag LC DefList RC {syntax_token_action($$, StructSpecifier, @$.first_line, 5, $1, $2, $3, $4, $5);}
               | STRUCT Tag {syntax_token_action($$, StructSpecifier, @$.first_line, 2, $1, $2);}
               ;
OptTag: ID {syntax_token_action($$, OptTag, @$.first_line, 1, $1);}
      | {$$ = NULL;}
      ;
Tag: ID {syntax_token_action($$, Tag, @$.first_line, 1, $1);}
   ;

VarDec: ID {syntax_token_action($$, VarDec, @$.first_line, 1, $1);}
      | VarDec LB INT RB {syntax_token_action($$, VarDec, @$.first_line, 4, $1, $2, $3, $4);}
      ;
FunDec: ID LP VarList RP {syntax_token_action($$, FunDec, @$.first_line, 4, $1, $2, $3, $4);}
      | ID LP RP {syntax_token_action($$, FunDec, @$.first_line, 3, $1, $2, $3);}
      ;
VarList: ParamDec COMMA VarList {syntax_token_action($$, VarList, @$.first_line, 3, $1, $2, $3);}
       | ParamDec {syntax_token_action($$, VarList, @$.first_line, 1, $1);}
       ;
ParamDec: Specifier VarDec {syntax_token_action($$, ParamDec, @$.first_line, 2, $1, $2);}
        ;

CompSt: LC DefList StmtList RC {syntax_token_action($$, CompSt, @$.first_line, 4, $1, $2, $3, $4);}
      ;
StmtList: Stmt StmtList {syntax_token_action($$, StmtList, @$.first_line, 2, $1, $2);}
        | {$$ = NULL;}
        ;
Stmt: Exp SEMI {syntax_token_action($$, Stmt, @$.first_line, 2, $1, $2);}
    | CompSt {syntax_token_action($$, Stmt, @$.first_line, 1, $1);}
    | RETURN Exp SEMI {syntax_token_action($$, Stmt, @$.first_line, 3, $1, $2, $3);}
    | IF LP Exp RP Stmt %prec LOWER_THAN_ELSE {syntax_token_action($$, Stmt, @$.first_line, 5, $1, $2, $3, $4, $5);}
    | IF LP Exp RP Stmt ELSE Stmt {syntax_token_action($$, Stmt, @$.first_line, 7, $1, $2, $3, $4, $5, $6, $7);}
    | WHILE LP Exp RP Stmt {syntax_token_action($$, Stmt, @$.first_line, 5, $1, $2, $3, $4, $5);}
    | error SEMI {err_occur = 1;}
    ;

DefList: Def DefList {syntax_token_action($$, DefList, @$.first_line, 2, $1, $2);}
       | {$$ = NULL;}
       ;
Def: Specifier DecList SEMI {syntax_token_action($$, Def, @$.first_line, 3, $1, $2, $3);}
   | Specifier error SEMI {err_occur = 1;}
   ;
DecList: Dec {syntax_token_action($$, DecList, @$.first_line, 1, $1);}
       | Dec COMMA DecList {syntax_token_action($$, DecList, @$.first_line, 3, $1, $2, $3);}
       ;
Dec: VarDec {syntax_token_action($$, Dec, @$.first_line, 1, $1);}
   | VarDec ASSIGNOP Exp {syntax_token_action($$, Dec, @$.first_line, 3, $1, $2, $3);}

   ;

Exp: Exp ASSIGNOP Exp {syntax_token_action($$, Exp, @$.first_line, 3, $1, $2, $3);}
   | Exp AND Exp {syntax_token_action($$, Exp, @$.first_line, 3, $1, $2, $3);}
   | Exp OR Exp {syntax_token_action($$, Exp, @$.first_line, 3, $1, $2, $3);}
   | Exp RELOP Exp {syntax_token_action($$, Exp, @$.first_line, 3, $1, $2, $3);}
   | Exp PLUS Exp {syntax_token_action($$, Exp, @$.first_line, 3, $1, $2, $3);}
   | Exp MINUS Exp {syntax_token_action($$, Exp, @$.first_line, 3, $1, $2, $3);}
   | Exp STAR Exp {syntax_token_action($$, Exp, @$.first_line, 3, $1, $2, $3);}
   | Exp DIV Exp {syntax_token_action($$, Exp, @$.first_line, 3, $1, $2, $3);}
   | LP Exp RP {syntax_token_action($$, Exp, @$.first_line, 3, $1, $2, $3);}
   | MINUS Exp {syntax_token_action($$, Exp, @$.first_line, 2, $1, $2);}
   | NOT Exp {syntax_token_action($$, Exp, @$.first_line, 2, $1, $2);}
   | ID LP Args RP {syntax_token_action($$, Exp, @$.first_line, 4, $1, $2, $3, $4);}
   | ID LP RP {syntax_token_action($$, Exp, @$.first_line, 3, $1, $2, $3);}
   | Exp LB Exp RB {syntax_token_action($$, Exp, @$.first_line, 4, $1, $2, $3, $4);}
   | Exp DOT ID {syntax_token_action($$, Exp, @$.first_line, 3, $1, $2, $3);}
   | ID {syntax_token_action($$, Exp, @$.first_line, 1, $1);}
   | INT {syntax_token_action($$, Exp, @$.first_line, 1, $1);}
   | FLOAT {syntax_token_action($$, Exp, @$.first_line, 1, $1);}
   ;
Args: Exp COMMA Args {syntax_token_action($$, Args, @$.first_line, 3, $1, $2, $3);}
    | Exp {syntax_token_action($$, Args, @$.first_line, 1, $1);}
    | error COMMA {err_occur = 1;}
    ;
%%
