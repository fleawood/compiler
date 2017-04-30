#ifndef _token_h_
#define _token_h_

enum {
	_INT,
	_FLOAT,
	_ID,
	_SEMI,
	_COMMA,
	_ASSIGNOP,
	_RELOP,
	_PLUS,
	_MINUS,
	_STAR,
	_DIV,
	_AND,
	_OR,
	_DOT,
	_NOT,
	_TYPE,
	_LP,
	_RP,
	_LB,
	_RB,
	_LC,
	_RC,
	_STRUCT,
	_RETURN,
	_IF,
	_ELSE,
	_WHILE,
	_Program,
	_ExtDefList,
	_ExtDef,
	_ExtDecList,
	_Specifier,
	_StructSpecifier,
	_OptTag,
	_Tag,
	_VarDec,
	_FunDec,
	_VarList,
	_ParamDec,
	_CompSt,
	_StmtList,
	_Stmt,
	_DefList,
	_Def,
	_DecList,
	_Dec,
	_Exp,
	_Args
};
static char *_token_name[] = {
"INT", "FLOAT", "ID", "SEMI", "COMMA", "ASSIGNOP", "RELOP", "PLUS", "MINUS", "STAR", "DIV", "AND", "OR", "DOT", "NOT", "TYPE", "LP", "RP", "LB", "RB", "LC", "RC", "STRUCT", "RETURN", "IF", "ELSE", "WHILE", "Program", "ExtDefList", "ExtDef", "ExtDecList", "Specifier", "StructSpecifier", "OptTag", "Tag", "VarDec", "FunDec", "VarList", "ParamDec", "CompSt", "StmtList", "Stmt", "DefList", "Def", "DecList", "Dec", "Exp", "Args"
};
#define token_name(token) _token_name[token]

#endif
