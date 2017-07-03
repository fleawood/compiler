#include <stdlib.h>
#include <stdio.h>
#include "Operand.h"

int temp_cnt = 0, var_cnt = 0, label_cnt = 0;
Operand _OPERAND_0 = { .type = OP_CONST, .value	= 0};
Operand _OPERAND_1 = { .type = OP_CONST, .value	= 1};

Operand* make_operand()
{
	Operand *op = malloc(sizeof(Operand));
	return op;
}

Operand* make_operand_variable()
{
	Operand *op = make_operand();
	op -> type = OP_VAR;
	op -> no = ++var_cnt;
	return op;
}

Operand* make_operand_constant(int value)
{
	Operand *op = make_operand();
	op -> type = OP_CONST;
	op -> value = value;
	return op;
}

Operand* make_operand_tempvar()
{
	Operand *op = make_operand();
	op -> type = OP_TEMP;
	op -> no = ++temp_cnt;
	return op;
}

Operand* make_operand_label()
{
	Operand *op = make_operand();
	op -> type = OP_LABEL;
	op -> no = ++label_cnt;
	return op;
}

Operand *make_operand_ref(char *var)
{
	Operand *op = make_operand();
	op -> type = OP_REF;
	op -> var = var;
	return op;
}

Operand *make_operand_deref(char *var)
{
	Operand *op = make_operand();
	op -> type = OP_DEREF;
	op -> var = var;
	return op;
}

char *operand_name(Operand *op)
{
	if (op == NULL) return "NULL";
	char *buffer;
	buffer = malloc(sizeof(char) * 10);
	switch (op -> type) {
		case OP_CONST:
			sprintf(buffer, "#%d", op -> value);
			break;
		case OP_LABEL:
			sprintf(buffer, "l%d", op -> no);
			break;
		case OP_VAR:
			sprintf(buffer, "v%d", op -> no);
			break;
		case OP_TEMP:
			sprintf(buffer, "t%d", op -> no);
			break;
		case OP_REF:
			sprintf(buffer, "&%s", op -> var);
			break;
		case OP_DEREF:
			sprintf(buffer, "*%s", op -> var);
			break;
	}
	return buffer;
}

OperandLink *make_operand_link(Operand *op)
{
	OperandLink *opl = malloc(sizeof(OperandLink));
	opl -> op = op;
	opl -> next = NULL;
	return opl;
}

OperandLink *bind_args(OperandLink *arg1, OperandLink *arg2)
{
	arg1 -> next = arg2;
	return arg1;
}
