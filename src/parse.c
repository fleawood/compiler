#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "parse.h"
#include "Type.h"
#include "Symbol.h"
#include "misc.h"
#include "link.h"

void parse_Program(Node *p);
void parse_ExtDefList(Node *p);
void parse_ExtDef(Node *p);
void parse_ExtDecList(Node *p, Type *type);
Type* parse_Specifier(Node *p);
Type* parse_StructSpecifier(Node *p);
char* parse_OptTag(Node *p);
char* parse_Tag(Node *p);
Symbol* parse_VarDec(Node *p, Type *type);
FuncInfo* parse_FunDec(Node *p, Type *type, bool is_def);
FuncInfo* parse_VarList(Node *p, Type *type);
Symbol* parse_ParamDec(Node *p);
void parse_CompSt(Node *p, FuncInfo *func);
void parse_StmtList(Node *p, Type *type);
void parse_Stmt(Node *p, Type *type);
void parse_DefList(Node *p, Type *s);
void parse_Def(Node *p, Type *s);
void parse_DecList(Node *p, Type *type, Type *s);
void parse_Dec(Node *p, Type *type, Type *s);
ExpType* parse_Exp(Node *p);
TypeLink* parse_Args(Node *p);

TypeLink* parse_Args(Node *p)
{
	assert(p -> node_type = _Args);
	TypeLink *types = NULL;
	Type *type = parse_Exp(child(p, 0)) -> type;
	TypeLink *type_link = make_type_link(type);
	insert_to_link(type_link, types, next);
	while (child_cnt(p) == 3) {
		assert(child_type(p, 1) == _COMMA);
		assert(child_type(p, 2) == _Args);
		p = child(p, 2);
		type = parse_Exp(child(p, 0)) -> type;
		type_link = make_type_link(type);
		insert_to_link(type_link, types, next);
	}
	return types;
}

ExpType* parse_Exp(Node *p)
{
	assert(p -> node_type == _Exp);
	ExpType *exp = NULL;
	ExpType *exp1, *exp2;
	ExpType *subscript;
	TypeLink *type_link;
	char *name;
	Symbol *symbol;
	switch (child_type(p, 0)) {
		case _Exp:
			exp1 = parse_Exp(child(p, 0));
			switch (child_type(p, 1)) {
				case _ASSIGNOP:
					exp2 = parse_Exp(child(p, 2));
					if (!exp1 -> is_lvalue) {
						parse_error(ERR_RVALUE_ASSIGN, p -> lineno, "lvalue required as left operand of assignment.");
						exp = err_exp_type;
					} else if (!is_type_equal(exp1 -> type, exp2 -> type)) {
						parse_error(ERR_INCOMP_ASSIGN, p -> lineno, "incompatible types in assignment.");
						exp = err_exp_type;
					} else {
						exp = make_exp_type(exp1 -> type, false);
					}
					break;
				case _AND:
				case _OR:
				case _RELOP:
					exp2 = parse_Exp(child(p, 2));
					if (!is_type_int(exp1 -> type) || !is_type_int(exp2 -> type)) {
						parse_error(ERR_INVALID_OPERAND, p -> lineno, "incompatible operand in relation operation.");
						exp = err_exp_type;
					} else {
						exp = make_exp_type(exp1 -> type, false);
					}
					break;
				case _PLUS:
				case _MINUS:
				case _STAR:
				case _DIV:
					exp2 = parse_Exp(child(p, 2));
					if (!is_type_int_or_float(exp1 -> type) || !is_type_equal(exp1 -> type, exp2 -> type)) {
						parse_error(ERR_INVALID_OPERAND, p -> lineno, "invalid operand in arithmetic operation.");
						exp = err_exp_type;
					} else {
						exp = make_exp_type(exp1 -> type, false);
					}
					break;
				case _LB:
					subscript = parse_Exp(child(p, 2));
					if (!is_type_array(exp1 -> type)) {
						parse_error(ERR_ARRAY_ACCESS, p -> lineno, "invalid array access.");
						exp = err_exp_type;
					} else if (!is_type_int(subscript -> type)) {
						parse_error(ERR_ARRAY_SUBSCRIPT, p -> lineno, "array subscript is not an integer.");
						exp = err_exp_type;
					} else {
						exp = make_exp_type(elem_type(exp1 -> type), exp1 -> is_lvalue);
					}
					break;
				case _DOT:
					assert(child_type(p, 2) == _ID);
					if (!is_type_struct(exp1 -> type)) {
						parse_error(ERR_FIELD_ACCESS, p -> lineno, "invalid field access.");
						exp = err_exp_type;
					} else {
						Field *field = find_field_in_struct(child(p, 2) -> svalue, exp1 -> type);
						if (field == NULL) {
							parse_error(ERR_FIELD_UNDEF, p -> lineno, "no field named \"%s\".", child(p, 2) -> svalue);
							exp = err_exp_type;
						} else {
							exp = make_exp_type(field_type(field), exp1 -> is_lvalue);
						}
					}
					break;
			}
			break;
		case _LP:
			return parse_Exp(child(p, 1));
			break;
		case _MINUS:
			exp1 = parse_Exp(child(p, 1));
			if (!is_type_int_or_float(exp1 -> type)) {
				parse_error(ERR_INVALID_OPERAND, p -> lineno, "invalid operand.");
				exp = err_exp_type;
			} else {
				exp = make_exp_type(exp1 -> type, false);
			}
			break;
		case _NOT:
			exp1 = parse_Exp(child(p, 1));
			if (!is_type_int(exp1 -> type)) {
				parse_error(ERR_INVALID_OPERAND, p -> lineno, "invalid operand.");
				exp = err_exp_type;
			} else {
				exp = make_exp_type(exp1 -> type, false);
			}
			break;
		case _ID:
			name = child(p, 0) -> svalue;
			symbol = find_symbol(name);
			switch (child_cnt(p)) {
				case 1:
					if (symbol == NULL) {
						parse_error(ERR_VAR_UNDEF, p -> lineno, "undefined variable \"%s\".", name);
						exp = err_exp_type;
					} else {
						exp = make_exp_type(symbol -> type, true);
					}
					break;
				case 3:
					if (symbol == NULL) {
						parse_error(ERR_FUNC_UNDEF, p -> lineno, "undefined function \"%s\".", name);
						exp = err_exp_type;
					} else if (!symbol -> is_func) {
						parse_error(ERR_FUNC_ACCESS, p -> lineno, "invalid function access.");
						exp = err_exp_type;
					} else {
						if (check_func_args(symbol -> func -> args, NULL) != 0) {
							parse_error(ERR_FUNC_ARGS, p -> lineno, "incompatible function arguments.");
							exp = err_exp_type;
						} else {
							exp = make_exp_type(symbol -> func -> type, false);
						}
					}
					break;
				case 4:
					type_link = parse_Args(child(p, 2));
					if (symbol == NULL) {
						parse_error(ERR_FUNC_UNDEF, p -> lineno, "undefined function \"%s\".", name);
						exp = err_exp_type;
					} else if (!symbol -> is_func) {
						parse_error(ERR_FUNC_ACCESS, p -> lineno, "invalid function access.");
						exp = err_exp_type;
					} else {
						if (check_func_args(symbol -> func -> args, type_link) != 0) {
							parse_error(ERR_FUNC_ARGS, p -> lineno, "incompatible function arguments.");
							exp = err_exp_type;
						} else {
							exp = make_exp_type(symbol -> func -> type, false);
						}
					}
					break;
			}
			break;
		case _INT:
			exp = make_exp_type(type_int, false);
			break;
		case _FLOAT:
			exp = make_exp_type(type_float, false);
			break;
	}
	assert(exp != NULL);
	return exp;
}

void parse_Dec(Node *p, Type *type, Type *s)
{
	assert(p -> node_type == _Dec);
	Symbol *symbol = parse_VarDec(child(p, 0), type);
	if (s != NULL) {
		insert_var_to_struct(symbol, s);
	} else {
		insert_var_to_stack(symbol, stack_top);
	}
	switch (child_cnt(p)) {
		case 1:
			break;
		case 3:
			assert(child_type(p, 1) == _ASSIGNOP);
			parse_Exp(child(p, 2));
			break;
		default:
			assert(0);
			break;
	}
}

void parse_DecList(Node *p, Type *type, Type *s)
{
	assert(p -> node_type == _DecList);
	parse_Dec(child(p, 0), type, s);
	while (child_cnt(p) == 3) {
		assert(child_type(p, 1) == _COMMA);
		assert(child_type(p, 2) == _DecList);
		p = child(p, 2);
		parse_Dec(child(p, 0), type, s);
	}
}

void parse_Def(Node *p, Type *s)
{
	assert(p -> node_type == _Def);
	Type *type = parse_Specifier(child(p, 0));
	parse_DecList(child(p, 1), type, s);
}

void parse_DefList(Node *p, Type *s)
{
	while (p != NULL) {
		assert(p -> node_type == _DefList);
		parse_Def(child(p, 0), s);
		p = child(p, 1);
	}
}

void parse_Stmt(Node *p, Type *type)
{
	assert(p -> node_type == _Stmt);
	ExpType *exp;
	switch (child_type(p, 0)) {
		case _Exp:
			parse_Exp(child(p, 0));
			break;
		case _CompSt:
			parse_CompSt(child(p, 0), NULL);
			break;
		case _RETURN:
			exp = parse_Exp(child(p, 1));
			if (!is_type_equal(exp -> type, type)) {
				parse_error(ERR_RETURN_TYPE, p -> lineno, "incompatible return type.");
			}
			break;
		case _IF:
			exp = parse_Exp(child(p, 2));
			if (!is_type_int(exp -> type)) {
				parse_error(ERR_INVALID_OPERAND, p -> lineno, "int value required in if condition.");
			}
			parse_Stmt(child(p, 4), type);
			switch (child_cnt(p)) {
				case 5:
					break;
				case 7:
					parse_Stmt(child(p, 6), type);
					break;
				default:
					assert(0);
					break;
			}
			break;
		case _WHILE:
			exp = parse_Exp(child(p, 2));
			if (!is_type_int(exp -> type)) {
				parse_error(ERR_INVALID_OPERAND, p -> lineno, "int value required in while condition.");
			}
			parse_Stmt(child(p, 4), type);
			break;
		default:
			assert(0);
			break;
	}
}

void parse_StmtList(Node *p, Type *type)
{
	while (p != NULL) {
		assert(p -> node_type == _StmtList);
		parse_Stmt(child(p, 0), type);
		p = child(p, 1);
	}
}

void parse_CompSt(Node *p, FuncInfo *func)
{
	assert(p -> node_type == _CompSt);
	push_stack();
	if (func != NULL) insert_func_args_to_stack(func, stack_top);
	parse_DefList(child(p, 1), NULL);
	parse_StmtList(child(p, 2), func == NULL ? NULL : func -> type);
	pop_stack();
}

Symbol* parse_ParamDec(Node *p)
{
	assert(p -> node_type == _ParamDec);
	Type *type = parse_Specifier(child(p, 0));
	return parse_VarDec(child(p, 1), type);
}

FuncInfo* parse_VarList(Node *p, Type *type)
{
	assert(p -> node_type == _VarList);
	FuncInfo *func = make_func_info(type);
	Symbol *symbol = parse_ParamDec(child(p, 0));
	insert_var_to_func_args(symbol, func);
	while (child_cnt(p) == 3) {
		assert(child_type(p, 1) == _COMMA);
		assert(child_type(p, 2) == _VarList);
		p = child(p, 2);
		symbol = parse_ParamDec(child(p, 0));
		insert_var_to_func_args(symbol, func);
	}
	return func;
}

FuncInfo* parse_FunDec(Node *p, Type *type, bool is_def)
{
	assert(p -> node_type == _FunDec);
	assert(child_type(p, 0) == _ID);
	FuncInfo *func;
	Symbol *symbol;
	switch (child_type(p, 2)) {
		case _VarList:
			func = parse_VarList(child(p, 2), type);
			break;
		case _RP:
			func = make_func_info(type);
			break;
		default:
			assert(0);
			break;
	}
	func -> is_def = is_def;
	symbol = make_func_symbol(func, child(p, 0) -> svalue, p -> lineno);
	if (is_def) insert_func_def_to_stack(symbol, stack_top);
	else insert_func_dec_to_stack(symbol, stack_top);
	return func;
}

Symbol *parse_VarDec(Node *p, Type *type)
{
	assert(p -> node_type == _VarDec);
	Symbol *symbol = NULL;
	Type *arraytype = NULL, *eletype;
	switch (child_type(p, 0)) {
		case _ID:
			symbol = make_var_symbol(type, child(p, 0) -> svalue, p -> lineno);
			break;
		case _VarDec:
			eletype = make_array_type(type, child(p, 2) -> ivalue);
			p = child(p, 0);
			while (child_type(p, 0) == _VarDec) {
				arraytype = make_array_type(eletype, child(p, 2) -> ivalue);
				eletype = arraytype;
				p = child(p, 0);
			}
			symbol = make_array_symbol(eletype, child(p, 0) -> svalue, p-> lineno);
			break;
		default:
			assert(0);
			break;
	}
	return symbol;
}

char* parse_Tag(Node *p)
{
	assert(p -> node_type == _Tag);
	assert(child_type(p, 0) == _ID);
	return child(p, 0) -> svalue;
}

char* parse_OptTag(Node *p)
{
	assert(p -> node_type == _OptTag);
	assert(child_type(p, 0) == _ID);
	return child(p, 0) -> svalue;
}

Type* parse_StructSpecifier(Node *p)
{
	assert(p -> node_type == _StructSpecifier);
	char *name;
	Symbol *symbol;
	Type *type;
	switch (child_type(p, 1)) {
		case _NULL:
			symbol = make_struct_symbol("$", p -> lineno);
			type = symbol -> type;
			parse_DefList(child(p, 3), type);
			break;
		case _OptTag:
			name = parse_OptTag(child(p, 1));
			symbol = make_struct_symbol(name, p -> lineno);
			insert_struct_to_stack(symbol, stack_top);
			type = symbol -> type;
			parse_DefList(child(p, 3), type);
			break;
		case _Tag:
			name = parse_Tag(child(p, 1));
			type = find_struct(name);
			if (type == NULL) {
				parse_error(ERR_STRUCT_UNDEF, p -> lineno, "undefined struct \"%s\".", name);
			}
			break;
		default:
			assert(0);
			break;
	}
	return type;
}

Type* parse_Specifier(Node *p)
{
	assert(p -> node_type == _Specifier);
	Type *type = NULL;
	switch (child_type(p, 0)) {
		case _TYPE:
			if (strcmp(child(p, 0) -> svalue, "int") == 0) {
				type = type_int;
			} else if (strcmp(child(p, 0) -> svalue, "float") == 0) {
				type = type_float;
			}
			break;
		case _StructSpecifier:
			type = parse_StructSpecifier(child(p, 0));
			break;
		default:
			assert(0);
			break;
	}
	return type;
}

void parse_ExtDecList(Node *p, Type *type)
{
	assert(p -> node_type == _ExtDecList);
	Symbol *symbol = parse_VarDec(child(p, 0), type);
	insert_var_to_stack(symbol, stack_top);
	while (child_cnt(p) == 3) {
		assert(child_type(p, 1) == _COMMA);
		assert(child_type(p, 2) == _ExtDecList);
		p = child(p, 2);
		symbol = parse_VarDec(child(p, 0), type);
		insert_var_to_stack(symbol, stack_top);
	}
}

void parse_ExtDef(Node *p)
{
	assert(p -> node_type == _ExtDef);
	Type *type = parse_Specifier(child(p, 0));
	FuncInfo *func;
	switch (child_type(p, 1)) {
		case _ExtDecList:
			parse_ExtDecList(child(p, 1), type);
			break;
		case _SEMI:
			break;
		case _FunDec:
			switch (child_type(p, 2)) {
				case _CompSt:
					func = parse_FunDec(child(p, 1), type, true);
					parse_CompSt(child(p, 2), func);
					break;
				case _SEMI:
					func = parse_FunDec(child(p, 1), type, false);
					break;
				default:
					assert(0);
					break;
			}
			break;
		default:
			assert(0);
			break;
	}
}

void parse_ExtDefList(Node *p)
{
	while (p != NULL) {
		assert(p -> node_type == _ExtDefList);
		parse_ExtDef(child(p, 0));
		p = child(p, 1);
	}
}

void parse_Program(Node *p)
{
	assert(p -> node_type == _Program);
	parse_ExtDefList(child(p, 0));
}

void parse_syntax(Node *p)
{
	hash_table_init();
	push_stack();
	parse_Program(p);
	check_func_def();
}
