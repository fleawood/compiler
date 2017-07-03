#ifndef _Operand_h_
#define _Operand_h_

typedef struct Operand Operand;

struct Operand {
	enum {
		OP_VAR,
		OP_CONST,
		OP_TEMP,
		OP_LABEL,
		OP_REF,
		OP_DEREF
	} type;
	union {
		int no;
		int value;
		char *var;
	};
};

typedef struct OperandLink OperandLink;

struct OperandLink {
	Operand *op;
	OperandLink *next;
};

extern int temp_cnt, var_cnt, label_cnt;

extern Operand _OPERAND_0, _OPERAND_1;
#define OPERAND_0 &_OPERAND_0
#define OPERAND_1 &_OPERAND_1

Operand* make_operand_variable();
Operand* make_operand_constant(int value);
Operand* make_operand_tempvar();
Operand* make_operand_label();
Operand *make_operand_ref(char *var);
Operand *make_operand_deref(char *var);

char *operand_name(Operand *op);

OperandLink *make_operand_link(Operand *op);

OperandLink *bind_args(OperandLink *arg1, OperandLink *arg2);

#endif
