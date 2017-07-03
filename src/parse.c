#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "parse.h"
#include "Type.h"
#include "Symbol.h"
#include "misc.h"
#include "link.h"
#include "translate.h"

InterCodeLink* parse_Program(Node *p);
InterCodeLink* parse_ExtDefList(Node *p);
InterCodeLink* parse_ExtDef(Node *p);
void parse_ExtDecList(Node *p, Type *type);
Type* parse_Specifier(Node *p);
Type* parse_StructSpecifier(Node *p);
char* parse_OptTag(Node *p);
char* parse_Tag(Node *p);
Symbol* parse_VarDec(Node *p, Type *type);
FuncInfo* parse_FunDec(Node *p, Type *type, bool is_def);
FuncInfo* parse_VarList(Node *p, Type *type);
Symbol* parse_ParamDec(Node *p);
InterCodeLink* parse_CompSt(Node *p, FuncInfo *func);
InterCodeLink* parse_StmtList(Node *p, Type *type);
InterCodeLink* parse_Stmt(Node *p, Type *type);
InterCodeLink* parse_DefList(Node *p, Type *s);
InterCodeLink* parse_Def(Node *p, Type *s);
InterCodeLink* parse_DecList(Node *p, Type *type, Type *s);
InterCodeLink* parse_Dec(Node *p, Type *type, Type *s);
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

InterCodeLink* parse_Dec(Node *p, Type *type, Type *s)
{
	assert(p -> node_type == _Dec);
	Symbol *symbol = parse_VarDec(child(p, 0), type);
	InterCodeLink *icl = NULL;
	InterCodeLink *icl1, *icl2;
	Operand *op;
	if (s != NULL) {
		insert_var_to_struct(symbol, s);
	} else {
		insert_var_to_stack(symbol, stack_top);
		icl = translate_VarDec(child(p, 0), type);
	}
	switch (child_cnt(p)) {
		case 1:
			break;
		case 3:
			assert(child_type(p, 1) == _ASSIGNOP);
			parse_Exp(child(p, 2));
			op = make_operand_tempvar();
			icl1 = translate_Exp(child(p, 2), &op);
			icl2 = make_intercode_link(assign, symbol -> op, op);
			icl = bind_code3(icl, icl1, icl2);
			if (s != NULL) {
				parse_error(ERR_FIELD_INIT, p -> lineno, "assignment in field \"%s\".", symbol -> name);
			}
			break;
		default:
			assert(0);
			break;
	}
	return icl;
}

InterCodeLink* parse_DecList(Node *p, Type *type, Type *s)
{
	assert(p -> node_type == _DecList);
	InterCodeLink *icl = NULL;
	icl = bind_code2(icl, parse_Dec(child(p, 0), type, s));
	while (child_cnt(p) == 3) {
		assert(child_type(p, 1) == _COMMA);
		assert(child_type(p, 2) == _DecList);
		p = child(p, 2);
		icl = bind_code2(icl, parse_Dec(child(p, 0), type, s));
	}
	return icl;
}

InterCodeLink* parse_Def(Node *p, Type *s)
{
	assert(p -> node_type == _Def);
	Type *type = parse_Specifier(child(p, 0));
	return parse_DecList(child(p, 1), type, s);
}

InterCodeLink* parse_DefList(Node *p, Type *s)
{
	InterCodeLink *icl = NULL;
	while (p != NULL) {
		assert(p -> node_type == _DefList);
		icl = bind_code2(icl, parse_Def(child(p, 0), s));
		p = child(p, 1);
	}
	return icl;
}

InterCodeLink* parse_Stmt(Node *p, Type *type)
{
	assert(p -> node_type == _Stmt);
	ExpType *exp;
	InterCodeLink *icl = NULL;
	InterCodeLink *icl1, *icl2, *icl3, *icl4, *icl5, *icl6, *icl7;
	Operand *op = NULL;
	Operand *label1, *label2, *label3;
	switch (child_type(p, 0)) {
		case _Exp:
			parse_Exp(child(p, 0));
			icl = translate_Exp(child(p, 0), &op);
			break;
		case _CompSt:
			icl = parse_CompSt(child(p, 0), NULL);
			break;
		case _RETURN:
			exp = parse_Exp(child(p, 1));
			if (!is_type_equal(exp -> type, type)) {
				parse_error(ERR_RETURN_TYPE, p -> lineno, "incompatible return type.");
			}
			op = make_operand_tempvar();
			icl1 = translate_Exp(child(p, 1), &op);
			icl2 = make_intercode_link(return, op);
			icl = bind_code2(icl1, icl2);
			break;
		case _IF:
			exp = parse_Exp(child(p, 2));
			if (!is_type_int(exp -> type)) {
				parse_error(ERR_INVALID_OPERAND, p -> lineno, "int value required in if condition.");
			}
			switch (child_cnt(p)) {
				case 5:
					label1 = make_operand_label();
					label2 = make_operand_label();
					icl1 = translate_Cond(child(p, 2), label1, label2);
					icl2 = make_intercode_link(label, label1);
					icl3 = parse_Stmt(child(p, 4), type);
					icl4 = make_intercode_link(label, label2);
					icl = bind_code4(icl1, icl2, icl3, icl4);
					break;
				case 7:
					label1 = make_operand_label();
					label2 = make_operand_label();
					label3 = make_operand_label();
					icl1 = translate_Cond(child(p, 2), label1, label2);
					icl2 = make_intercode_link(label, label1);
					icl3 = parse_Stmt(child(p, 4), type);
					icl4 = make_intercode_link(goto, label3);
					icl5 = make_intercode_link(label, label2);
					icl6 = parse_Stmt(child(p, 6), type);
					icl7 = make_intercode_link(label, label3);
					icl = bind_code7(icl1, icl2, icl3, icl4, icl5, icl6, icl7);
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
			label1 = make_operand_label();
			label2 = make_operand_label();
			label3 = make_operand_label();
			icl1 = make_intercode_link(label, label1);
			icl2 = translate_Cond(child(p, 2), label2, label3);
			icl3 = make_intercode_link(label, label2);
			icl4 = parse_Stmt(child(p, 4), type);
			icl5 = make_intercode_link(goto, label1);
			icl6 = make_intercode_link(label, label3);
			icl = bind_code6(icl1, icl2, icl3, icl4, icl5, icl6);
			break;
		default:
			assert(0);
			break;
	}
	return icl;
}

InterCodeLink* parse_StmtList(Node *p, Type *type)
{
	InterCodeLink *icl = NULL;
	while (p != NULL) {
		assert(p -> node_type == _StmtList);
		icl = bind_code2(icl, parse_Stmt(child(p, 0), type));
		p = child(p, 1);
	}
	return icl;
}

InterCodeLink* parse_CompSt(Node *p, FuncInfo *func)
{
	assert(p -> node_type == _CompSt);
	InterCodeLink *icl = NULL;
	InterCodeLink *icl1, *icl2;
	push_stack();
	if (func != NULL) insert_func_args_to_stack(func, stack_top);
	icl1 = parse_DefList(child(p, 1), NULL);
	icl2 = parse_StmtList(child(p, 2), func == NULL ? NULL : func -> type);
	icl = bind_code2(icl1, icl2);
	pop_stack();
	return icl;
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

Symbol* parse_VarDec(Node *p, Type *type)
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

InterCodeLink* parse_ExtDef(Node *p)
{
	assert(p -> node_type == _ExtDef);
	Type *type = parse_Specifier(child(p, 0));
	InterCodeLink *icl = NULL, *icl1, *icl2;
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
					icl1 = translate_FunDec(child(p, 1), func);
					icl2 = parse_CompSt(child(p, 2), func);
					icl = bind_code2(icl1, icl2);
					break;
				case _SEMI:
					parse_FunDec(child(p, 1), type, false);
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
	return icl;
}

InterCodeLink* parse_ExtDefList(Node *p)
{
	InterCodeLink *icl = NULL;
	while (p != NULL) {
		assert(p -> node_type == _ExtDefList);
		icl = bind_code2(icl, parse_ExtDef(child(p, 0)));
		p = child(p, 1);
	}
	return icl;
}

InterCodeLink* parse_Program(Node *p)
{
	assert(p -> node_type == _Program);
	return parse_ExtDefList(child(p, 0));
}

void load_read()
{
	FuncInfo *func = make_func_info(type_int);
	func -> is_def = true;
	Symbol *symbol = make_func_symbol(func, "read", -1);
	insert_func_def_to_stack(symbol, stack_top);
}

void load_write()
{
	FuncInfo *func = make_func_info(type_int);
	func -> is_def = true;
	Symbol *arg = make_var_symbol(type_int, "n", -1);
	insert_var_to_func_args(arg, func);
	Symbol *symbol = make_func_symbol(func, "write", -1);
	insert_func_def_to_stack(symbol, stack_top);
}

void load_pre_define_func()
{
	load_read();
	load_write();
}

InterCodeLink* parse_syntax(Node *p)
{
	InterCodeLink *icl;
	hash_table_init();
	push_stack();
	load_pre_define_func();
	icl = parse_Program(p);
	check_func_def();
	return icl;
}
