#include <stdlib.h>
#include <string.h>
#include "translate.h"

InterCodeLink* translate_Addr(Node *p, Type **type, Operand **op)
{
	InterCodeLink *icl = NULL;
	InterCodeLink *icl1, *icl2, *icl3, *icl4;
	Operand *op1, *op2, *op3;
	char *name;
	Field *field;
	switch (child_type(p, 0)) {
		case _ID:
			if (*op != NULL) free(*op);
			name = child(p, 0) -> svalue;
			*op = make_operand_ref(name);
			*type = find_symbol(name) -> type;
			break;
		case _Exp:
			switch (child_type(p, 1)) {
				case _DOT:
					op1 = make_operand_tempvar();
					icl1 = translate_Addr(child(p, 0), type, &op1);
					field = find_field_in_struct(child(p, 2) -> svalue, *type);
					icl2 = make_intercode_link(arith, _PLUS, *op, op1, make_operand_constant(field -> offset));
					icl = bind_code2(icl1, icl2);
					*type = field -> symbol -> type;
					break;
				case _LB:
					op1 = make_operand_tempvar();
					op2 = make_operand_tempvar();
					op3 = make_operand_tempvar();
					icl1 = translate_Addr(child(p, 0), type, &op1);
					icl2 = translate_Exp(child(p, 2), &op2);
					icl3 = make_intercode_link(arith, _STAR, op3, op2, make_operand_constant((*type) -> _array -> elem -> size));
					icl4 = make_intercode_link(arith, _PLUS, *op, op1, op3);
					icl = bind_code4(icl1, icl2, icl3, icl4);
					*type = (*type) -> _array -> elem;
					break;
			}
			break;
	}
	return icl;
}

InterCodeLink* translate_Exp(Node *p, Operand **op)
{
	int value;
	Operand *op1 = NULL, *op2 = NULL, *op3 = NULL;
	Operand *label1, *label2;
	InterCodeLink *icl = NULL;
	InterCodeLink *icl1, *icl2, *icl3, *icl4;
	Symbol *symbol;
	OperandLink *args;
	Type *type = NULL;
	switch (child_type(p, 0)) {
		case _INT:
			value = child(p, 0) -> ivalue;
			if (*op != NULL) free(*op);
			*op = make_operand_constant(value);
			//icl = make_intercode_link(assign, op, make_operand_constant(value));
			break;
		case _ID:
			switch (child_cnt(p)) {
				case 1:
					op1 = find_symbol(child(p, 0) -> svalue) -> op;
					if (*op != NULL) free(*op);
					*op = op1;
					//icl = make_intercode_link(assign, op, op1);
					break;
				case 3:
					symbol = find_symbol(child(p, 0) -> svalue);
					if (strcmp(symbol -> name, "read") == 0) {
						icl = make_intercode_link(read, *op);
					} else {
						icl = make_intercode_link(call, *op, symbol -> name);
					}
					break;
				case 4:
					symbol = find_symbol(child(p, 0) -> svalue);
					args = NULL;
					icl1 = translate_Args(child(p, 2), &args);
					if (strcmp(symbol -> name, "write") == 0) {
						icl2 = make_intercode_link(write, args -> op);
						icl = bind_code2(icl1, icl2);
					} else {
						icl2 = NULL;
						for (OperandLink *arg = args; arg != NULL; arg = arg -> next) {
							icl2 = bind_code2(icl2, make_intercode_link(arg, arg -> op));
						};
						icl3 = make_intercode_link(call, *op, symbol -> name);
						icl = bind_code3(icl1, icl2, icl3);
					}
					break;
			}
			break;
		case _Exp:
			switch (child_type(p, 1)) {
				case _ASSIGNOP:
					if (child_type(child(p, 0), 0) == _ID) {
						op1 = make_operand_tempvar();
						icl1 = translate_Exp(child(p, 0), &op1);
						op2 = make_operand_tempvar();
						icl2 = translate_Exp(child(p, 2), &op2);
						icl3 = make_intercode_link(assign, op1, op2);
						icl4 = make_intercode_link(assign, *op, op1);
						icl = bind_code4(icl1, icl2, icl3, icl4);
					} else {
						op1 = make_operand_tempvar();
						icl1 = translate_Addr(child(p, 0), &type, &op1);
						op2 = make_operand_deref(operand_name(op1));
						op3 = make_operand_tempvar();
						icl2 = translate_Exp(child(p, 2), &op3);
						icl3 = make_intercode_link(assign, op2, op3);
						icl4 = make_intercode_link(assign, *op, op2);
						icl = bind_code4(icl1, icl2, icl3, icl4);
					}
					break;
				case _PLUS:
				case _MINUS:
				case _STAR:
				case _DIV:
					op1 = make_operand_tempvar();
					op2 = make_operand_tempvar();
					icl1 = translate_Exp(child(p, 0), &op1);
					icl2 = translate_Exp(child(p, 2), &op2);
					icl3 = make_intercode_link(arith, child_type(p, 1), *op, op1, op2);
					icl = bind_code3(icl1, icl2, icl3);
					break;
				case _RELOP:
				case _AND:
				case _OR:
					label1 = make_operand_label();
					label2 = make_operand_label();
					icl1 = make_intercode_link(assign, *op, OPERAND_0);
					icl2 = translate_Cond(p, label1, label2);
					icl3 = make_intercode_link(label, label1);
					icl4 = make_intercode_link(assign, *op, OPERAND_1);
					icl = bind_code4(icl1, icl2, icl3, icl4);
					break;
				case _DOT:
				case _LB:
					op1 = make_operand_tempvar();
					icl1 = translate_Addr(p, &type, &op1);
					op2 = make_operand_deref(operand_name(op1));
					icl2 = make_intercode_link(assign, *op, op2);
					icl = bind_code2(icl1, icl2);
					break;
			}
			break;
		case _MINUS:
			op1 = make_operand_tempvar();
			icl1 = translate_Exp(child(p, 1), &op1);
			icl2 = make_intercode_link(arith, _MINUS, *op, OPERAND_0, op1);
			icl = bind_code2(icl1, icl2);
			break;
		case _NOT:
			label1 = make_operand_label();
			label2 = make_operand_label();
			icl1 = make_intercode_link(assign, *op, OPERAND_0);
			icl2 = translate_Cond(p, label1, label2);
			icl3 = make_intercode_link(label, label1);
			icl4 = make_intercode_link(assign, *op, OPERAND_1);
			icl = bind_code4(icl1, icl2, icl3, icl4);
			break;
		case _LP:
			icl = translate_Exp(child(p, 1), op);
			break;
	}
	return icl;
}

/*
InterCodeLink* translate_Stmt(Node *p)
{
	InterCodeLink *icl, *icl1, *icl2, *icl3, *icl4, *icl5, *icl6, *icl7;
	Operand *op1, *op2;
	Operand *label1, *label2, *label3;
	Operand *op = NULL;
	switch (child_type(p, 0)) {
		case _Exp:
			icl = translate_Exp(child(p, 0), &op);
			break;
		case _CompSt:
			icl = translate_CompSt(child(p, 0));
			break;
		case _RETURN:
			op1 = make_operand_tempvar();
			icl1 = translate_Exp(child(p, 1), &op1);
			icl2 = make_intercode_link(return, op1);
			icl = bind_code2(icl1, icl2);
			break;
		case _IF:
			switch (child_cnt(p)) {
				case 5:
					label1 = make_operand_label();
					label2 = make_operand_label();
					icl1 = translate_Cond(child(p, 2), label1, label2);
					icl2 = make_intercode_link(label, label1);
					icl3 = translate_Stmt(child(p, 4));
					icl4 = make_intercode_link(label, label2);
					icl = bind_code4(icl1, icl2, icl3, icl4);
					break;
				case 7:
					label1 = make_operand_label();
					label2 = make_operand_label();
					label3 = make_operand_label();
					icl1 = translate_Cond(child(p, 2), label1, label2);
					icl2 = make_intercode_link(label, label1);
					icl3 = translate_Stmt(child(p, 4));
					icl4 = make_intercode_link(goto, label3);
					icl5 = make_intercode_link(label, label2);
					icl6 = translate_Stmt(child(p, 6));
					icl7 = make_intercode_link(label, label3);
					icl = bind_code7(icl1, icl2, icl3, icl4, icl5, icl6, icl7);
					break;
				default:
					break;
			}
			break;
		case _WHILE:
			label1 = make_operand_label();
			label2 = make_operand_label();
			label3 = make_operand_label();
			icl1 = make_intercode_link(label, label1);
			icl2 = translate_Cond(child(p, 2), label2, label3);
			icl3 = make_intercode_link(label, label2);
			icl4 = translate_Stmt(child(p, 4));
			icl5 = make_intercode_link(goto, label1);
			icl6 = make_intercode_link(label, label3);
			icl = bind_code6(icl1, icl2, icl3, icl4, icl5, icl6);
			break;
	}
	return icl;
}
*/

InterCodeLink* translate_Cond(Node *p, Operand *label_true, Operand *label_false)
{
	InterCodeLink *icl = NULL;
	InterCodeLink *icl1, *icl2, *icl3, *icl4;
	Operand *op1, *op2;
	Operand *label1;
	int rel;
	switch (p -> node_type) {
		case _Exp:
			switch (child_type(p, 1)) {
				case _RELOP:
					op1 = make_operand_tempvar();
					op2 = make_operand_tempvar();
					icl1 = translate_Exp(child(p, 0), &op1);
					icl2 = translate_Exp(child(p, 2), &op2);
					rel = child(p, 1) -> reltype;
					icl3 = make_intercode_link(if, op1, rel, op2, label_true);
					icl4 = make_intercode_link(goto, label_false);
					icl = bind_code4(icl1, icl2, icl3, icl4);
					break;
				case _AND:
					label1 = make_operand_label();
					icl1 = translate_Cond(child(p, 0), label1, label_false);
					icl2 = make_intercode_link(label, label1);
					icl3 = translate_Cond(child(p, 2), label_true, label_false);
					icl = bind_code3(icl1, icl2, icl3);
					break;
				case _OR:
					label1 = make_operand_label();
					icl1 = translate_Cond(child(p, 0), label_true, label1);
					icl2 = make_intercode_link(label, label1);
					icl3 = translate_Cond(child(p, 0), label_true, label_false);
					icl = bind_code3(icl1, icl2, icl3);
					break;
			}
			break;
		case _NOT:
			icl = translate_Cond(child(p, 1), label_false, label_true);
			break;
		default:
			op1 = make_operand_tempvar();
			icl1 = translate_Exp(p, &op1);
			icl2 = make_intercode_link(if, op1, RELOP_NE, OPERAND_0, label_true);
			icl3 = make_intercode_link(goto, label_false);
			icl = bind_code3(icl1, icl2, icl3);
			break;
	}
	return icl;
}

InterCodeLink* translate_Args(Node *p, OperandLink **args)
{
	Operand *op;
	InterCodeLink *icl = NULL;
	InterCodeLink *icl1, *icl2;
	switch (child_cnt(p)) {
		case 1:
			op = make_operand_tempvar();
			icl = translate_Exp(child(p, 0), &op);
			*args = bind_args(make_operand_link(op), *args);
			break;
		case 3:
			op = make_operand_tempvar();
			icl1 = translate_Exp(child(p, 0), &op);
			*args = bind_args(make_operand_link(op), *args);
			icl2 = translate_Args(child(p, 2), args);
			icl = bind_code2(icl1, icl2);
			break;
	}
	return icl;
}
/*
Type *get_struct_specifier_type(Node *p)
{
	// due to some reasons, i cannot deal with anonymous struct
	// it can be solved by adding extra data, but i hate to do so
	Symbol *symbol = find_symbol(child(child(p, 1), 0));
	return symbol -> type;
}

Type* get_specifier_type(Node *p)
{
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
			type = get_struct_specifier_type(child(p, 0));
			break;
	}
	return type;
}


InterCodeLink* translate_Def(Node *p)
{
	Type *type = get_specifier_type(child(p, 0));
}

InterCodeLink* translate_DefList(Node *p)
{
	if (p == NULL) return NULL;
	InterCodeLink *icl1, *icl2;
	icl1 = translate_Def(child(p, 0));
	icl2 = translate_DefList(child(p, 1));
	return = bind_code2(icl1, icl2);
}

InterCodeLink* translate_CompSt(Node *p)
{
	InterCodeLink *icl1, *icl2;
	icl1 = translate_DefList();
	icl2 = translate_StmtList();
	return bind_code2(icl1, icl2);
}*/

InterCodeLink* translate_FunDec(Node *p, FuncInfo *func)
{
	InterCodeLink *icl = NULL;
	InterCodeLink *icl1, *icl2;
	icl1 = make_intercode_link(func, child(p, 0) -> svalue);
	icl2 = NULL;
	for (ArgLink *args = func -> args; args != NULL; args = args -> next) {
		icl2 = bind_code2(icl2, make_intercode_link(param, args -> arg -> op));
	}
	icl = bind_code2(icl1, icl2);
	return icl;
}

InterCodeLink* translate_VarDec(Node *p, Type *type)
{
	InterCodeLink *icl = NULL;
	Symbol *symbol;
	switch (child_type(p, 0)) {
		case _ID:
			if (is_type_struct(type)) {
				icl = make_intercode_link(dec, child(p, 0) -> svalue, type -> size);
			}
			break;
		case _VarDec:
			while (child_type(p, 0) == _VarDec) p = child(p, 0);
			symbol = find_symbol(child(p, 0) -> svalue);
			icl = make_intercode_link(dec, child(p, 0) -> svalue, symbol -> type -> size);
			break;
	}
	return icl;
}
