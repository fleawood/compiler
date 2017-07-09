#ifndef _InterCode_h_
#define _InterCode_h_

#include <stdio.h>

typedef struct InterCode InterCode;
typedef struct InterCodeLink InterCodeLink;

#include "Operand.h"

struct InterCode {
	enum {
		IC_LABEL,
		IC_FUNC,
		IC_ASSIGN,
		IC_ADD,
		IC_SUB,
		IC_MUL,
		IC_DIV,
		IC_RREF,
		IC_RDEREF,
		IC_LDEREF,
		IC_GOTO,
		IC_IF,
		IC_RETURN,
		IC_DEC,
		IC_ARG,
		IC_CALL,
		IC_PARAM,
		IC_READ,
		IC_WRITE
	} type;
	union {
		struct {
			Operand *unary;
		};
		struct {
			char *name;
		};
		struct {
			Operand *left, *right;
		};
		struct {
			Operand *rv;
			char *func;
		};
		struct {
			Operand *result, *op1, *op2;
		};
		struct {
			Operand *src1;
			int reltype;
			Operand *src2;
			Operand *dest;
		};
		struct {
			Operand *array;
			int size;
		};
	};
};

struct InterCodeLink {
	InterCode *ic;
	InterCodeLink *prev, *next;
};

InterCode* make_intercode_unary(int type, Operand *op);
InterCode* make_intercode_binary(int type, Operand *left, Operand *right);
InterCode* make_intercode_if(Operand *src1, int reltype, Operand *src2, Operand *dest);
InterCode* make_intercode_arith(int type, Operand *result, Operand *op1, Operand *op2);
InterCode* make_intercode_call(Operand *rv, char *func);
InterCode* make_intercode_func(char *name);
InterCode* make_intercode_dec(Operand *array, int size);

#define make_intercode_arg(arg) make_intercode_unary(IC_ARG, arg)
#define make_intercode_label(label) make_intercode_unary(IC_LABEL, label)
#define make_intercode_goto(label) make_intercode_unary(IC_GOTO, label)
#define make_intercode_return(op) make_intercode_unary(IC_RETURN, op)
#define make_intercode_param(arg) make_intercode_unary(IC_PARAM, arg)
#define make_intercode_read(op) make_intercode_unary(IC_READ, op)
#define make_intercode_write(op) make_intercode_unary(IC_WRITE, op)

#define make_intercode_assign(left, right) make_intercode_binary(IC_ASSIGN, left, right)


InterCodeLink* make_intercode_to_link(InterCode *ic);
#define make_intercode_link(func, ...) make_intercode_to_link(make_intercode_##func(__VA_ARGS__))

InterCodeLink* bind_code(InterCodeLink *icl1, InterCodeLink *icl2);
#define bind_code2(icl1, icl2) bind_code(icl1, icl2)
#define bind_code3(icl1, icl2, icl3) bind_code(bind_code2(icl1, icl2), icl3)
#define bind_code4(icl1, icl2, icl3, icl4) bind_code(bind_code3(icl1, icl2, icl3), icl4)
#define bind_code5(icl1, icl2, icl3, icl4, icl5) bind_code(bind_code4(icl1, icl2, icl3, icl4), icl5)
#define bind_code6(icl1, icl2, icl3, icl4, icl5, icl6) bind_code(bind_code5(icl1, icl2, icl3, icl4, icl5), icl6)
#define bind_code7(icl1, icl2, icl3, icl4, icl5, icl6, icl7) bind_code(bind_code6(icl1, icl2, icl3, icl4, icl5, icl6), icl7)

void print_icl(FILE *f, InterCodeLink *icl);

#endif
