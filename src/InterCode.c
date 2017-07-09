#include <stdlib.h>
#include <stdio.h>
#include "InterCode.h"
#include "Node.h"

InterCode* make_intercode()
{
	InterCode *ic = malloc(sizeof(InterCode));
	return ic;
}

InterCode* make_intercode_arith(int type, Operand *result, Operand *op1, Operand *op2)
{
	InterCode *ic = make_intercode();
	switch (type) {
		case _PLUS:
			ic -> type = IC_ADD;
			break;
		case _MINUS:
			ic -> type = IC_SUB;
			break;
		case _STAR:
			ic -> type = IC_MUL;
			break;
		case _DIV:
			ic -> type = IC_DIV;
			break;
	}
	ic -> op1 = op1;
	ic -> op2 = op2;
	ic -> result = result;
	return ic;
}

InterCode* make_intercode_if(Operand *src1, int reltype, Operand *src2, Operand *dest)
{
	InterCode *ic = make_intercode();
	ic -> type = IC_IF;
	ic -> src1 = src1;
	ic -> reltype = reltype;
	ic -> src2 = src2;
	ic -> dest = dest;
	return ic;
}

InterCode* make_intercode_call(Operand *rv, char *func)
{
	InterCode *ic = make_intercode();
	ic -> type = IC_CALL;
	ic -> rv = rv;
	ic -> func = func;
	return ic;
}

InterCode* make_intercode_func(char *name)
{
	InterCode *ic = make_intercode();
	ic -> type = IC_FUNC;
	ic -> name = name;
	return ic;
}

InterCode* make_intercode_dec(Operand *array, int size)
{
	InterCode *ic = make_intercode();
	ic -> type = IC_DEC;
	ic -> array = array;
	ic -> size = size;
	return ic;
}

InterCode* make_intercode_unary(int type, Operand *op)
{
	InterCode *ic = make_intercode();
	ic -> type = type;
	ic -> unary = op;
	return ic;
}

InterCode* make_intercode_binary(int type, Operand *left, Operand *right)
{
	if (left == NULL) return NULL;
	InterCode *ic = make_intercode();
	ic -> type = type;
	ic -> left = left;
	ic -> right = right;
	return ic;
}

InterCodeLink* make_intercode_to_link(InterCode *ic)
{
	if (ic == NULL) return NULL;
	InterCodeLink *icl = malloc(sizeof(InterCodeLink));
	icl -> ic = ic;
	icl -> prev = icl -> next = icl;
	return icl;
}

InterCodeLink* bind_code(InterCodeLink *icl1, InterCodeLink *icl2)
{
	if (icl1 == NULL) return icl2;
	if (icl2 == NULL) return icl1;
	InterCodeLink *head1, *tail1, *head2, *tail2;
	head1 = icl1;
	tail1 = icl1 -> prev;
	head2 = icl2;
	tail2 = icl2 -> prev;

	tail1 -> next = head2;
	head2 -> prev = tail1;
	tail2 -> next = head1;
	head1 -> prev = tail2;
	return head1;
}

void print_ic(FILE *f, InterCode *ic)
{
	switch (ic -> type) {
		case IC_LABEL:
			fprintf(f, "LABEL %s :\n", operand_name(ic -> unary));
			break;
		case IC_FUNC:
			fprintf(f, "FUNCTION %s :\n", ic -> name);
			break;
		case IC_ASSIGN:
			fprintf(f, "%s := %s\n", operand_name(ic -> left), operand_name(ic -> right));
			break;
		case IC_ADD:
			fprintf(f, "%s := %s + %s\n", operand_name(ic -> result), operand_name(ic -> op1), operand_name(ic -> op2));
			break;
		case IC_SUB:
			fprintf(f, "%s := %s - %s\n", operand_name(ic -> result), operand_name(ic -> op1), operand_name(ic -> op2));
			break;
		case IC_MUL:
			fprintf(f, "%s := %s * %s\n", operand_name(ic -> result), operand_name(ic -> op1), operand_name(ic -> op2));
			break;
		case IC_DIV:
			fprintf(f, "%s := %s / %s\n", operand_name(ic -> result), operand_name(ic -> op1), operand_name(ic -> op2));
			break;
		case IC_RREF:
			fprintf(f, "%s := &%s\n", operand_name(ic -> left), operand_name(ic -> right));
			break;
		case IC_RDEREF:
			fprintf(f, "%s := *%s\n", operand_name(ic -> left), operand_name(ic -> right));
			break;
		case IC_LDEREF:
			fprintf(f, "*%s := %s\n", operand_name(ic -> left), operand_name(ic -> right));
			break;
		case IC_GOTO:
			fprintf(f, "GOTO %s\n", operand_name(ic -> unary));
			break;
		case IC_IF:
			fprintf(f, "IF %s %s %s GOTO %s\n", operand_name(ic -> src1), relop_name(ic -> reltype), operand_name(ic -> src2), operand_name(ic -> dest));
			break;
		case IC_RETURN:
			fprintf(f, "RETURN %s\n", operand_name(ic -> unary));
			break;
		case IC_DEC:
			fprintf(f, "DEC %s %d\n", operand_name(ic -> array), ic -> size);
			break;
		case IC_ARG:
			fprintf(f, "ARG %s\n", operand_name(ic -> unary));
			break;
		case IC_CALL:
			fprintf(f, "%s := CALL %s\n", operand_name(ic -> rv), ic -> func);
			break;
		case IC_PARAM:
			fprintf(f, "PARAM %s\n", operand_name(ic -> unary));
			break;
		case IC_READ:
			fprintf(f, "READ %s\n", operand_name(ic -> unary));
			break;
		case IC_WRITE:
			fprintf(f, "WRITE %s\n", operand_name(ic -> unary));
			break;
	}
}

void print_icl(FILE *f, InterCodeLink *icl)
{
	if (icl == NULL) return;
	InterCodeLink *head = icl;
	do {
		print_ic(f, icl -> ic);
		icl = icl -> next;
	} while (icl != head);
}
