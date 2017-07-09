#include <string.h>
#include "generate.h"
#include "Symbol.h"
#include "InterCode.h"

static char init_code[] =
{
	".data\n"
	"_prompt: .asciiz \"Enter an integer:\"\n"
	"_ret: .asciiz \"\\n\"\n"
	".globl main\n"
	".text\n"

	"read:\n"
	" li $v0, 4\n"
	" la $a0, _prompt\n"
	" syscall\n"
	" li $v0, 5\n"
	" syscall\n"
	" jr $ra\n"

	"write:\n"
	" li $v0, 1\n"
	" syscall\n"
	" li $v0, 4\n"
	" la $a0, _ret\n"
	" syscall\n"
	" move $v0, $s0\n"
	" jr $ra\n"
};

static char reg_name[][4] =
{
	"$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7", "$t8", "$t9"
};

static bool used[10];
static Operand *fop;

void calc_offset(Operand *op)
{
	if (op == NULL) return;
	if (op -> mark) return;
	if (op -> type != OP_VAR && op -> type != OP_TEMP) return;
	op -> mark = true;
	fop -> size += 4;
	op -> offset = -fop -> size;
}

void proc_icl(InterCodeLink *icl)
{
	if (icl == NULL) return;
	InterCodeLink *head = icl;
	int num_arg = 0;
	do
	{
		InterCode *ic = icl -> ic;
		switch (ic -> type)
		{
			case IC_FUNC:
				fop = find_symbol(ic -> name) -> op;
				fop -> size = 4;
				num_arg = 0;
				break;
			case IC_ASSIGN:
				calc_offset(ic -> left);
				calc_offset(ic -> right);
				break;
			case IC_ADD:
			case IC_SUB:
			case IC_MUL:
			case IC_DIV:
				calc_offset(ic -> result);
				calc_offset(ic -> op1);
				calc_offset(ic -> op2);
				break;
			case IC_CALL:
				calc_offset(ic -> rv);
				break;
			case IC_PARAM:
				if (num_arg < 4) calc_offset(ic -> unary);
				else {
					ic -> unary -> mark = true;
					ic -> unary -> offset = (num_arg - 3) * 4;
				}
				num_arg ++;
				break;
			case IC_DEC:
				ic -> array -> mark = true;
				fop -> size += ic -> size;
				ic -> array -> offset = -fop -> size;
				break;
		}
		icl = icl -> next;
	} while (icl != head);
}

void clear_reg()
{
	memset(used, 0, sizeof used);
}

char *reg(FILE *f, Operand *op)
{
	for (int i = 0; i < 10; ++i)
	{
		if (!used[i])
		{
			used[i] = true;
			if (op -> type == OP_CONST)
			{
				fprintf(f, " li $t%d, %d\n", i, op -> value);
			}
			else if (op -> type == OP_REF)
			{
				fprintf(f, " la $t%d, %d($fp)\n", i, op -> ori -> offset);
			}
			else if (op -> type == OP_DEREF)
			{
				fprintf(f, " lw $t%d, %d($fp)\n", i, op -> ori -> offset);
			}
			else
			{
				fprintf(f, " lw $t%d, %d($fp)\n", i, op -> offset);
			}
			return reg_name[i];
		}
	}
}

void print_icl2code(FILE *f, InterCodeLink *icl)
{
	if (icl == NULL) return;
	fputs(init_code, f);
	InterCodeLink *head = icl;
	int num_arg = 0, num_param = 0;
	int size;
	char *reg1, *reg2, *reg3;
	InterCodeLink *icl1;
	int i;
	do
	{
		InterCode *ic = icl -> ic;
		Symbol *symbol;
		clear_reg();
		switch (ic -> type) {
			case IC_LABEL:
				fprintf(f, "%s:\n", operand_name(ic -> unary));
				break;
			case IC_FUNC:
				size = find_symbol(ic -> name) -> op -> offset;
				num_param = 0;
				fprintf(f, "%s:\n", ic -> name);
				fprintf(f, " subu $sp, $sp, %d\n", size);
				fprintf(f, " sw $fp, %d($sp)\n", size - 4);
				fprintf(f, " addi $fp, $sp, %d\n", size);
				break;
			case IC_ASSIGN:
				if (ic -> right -> type == OP_DEREF)
				{
					reg1 = reg(f, ic -> left);
					reg2 = reg(f, ic -> right);
					fprintf(f, " lw %s, 0(%s)\n", reg1, reg2);
					fprintf(f, " sw %s, %d($fp)\n", reg1, ic -> left -> offset);
				}
				else if (ic -> left -> type == OP_DEREF)
				{
					reg1 = reg(f, ic -> left);
					reg2 = reg(f, ic -> right);
					fprintf(f, " sw %s, 0(%s)\n", reg2, reg1);
				}
				else
				{
					reg2 = reg(f, ic -> right);
					fprintf(f, " sw %s, %d($fp)\n", reg2, ic -> left -> offset);
				}
				break;
			case IC_ADD:
				if (ic -> op2 -> type == OP_CONST)
				{
					reg1 = reg(f, ic -> result);
					reg2 = reg(f, ic -> op1);
					fprintf(f, " addi %s, %s, %d\n", reg1, reg2, ic -> op2 -> value);
				}
				else
				{
					reg1 = reg(f, ic -> result);
					reg2 = reg(f, ic -> op1);
					reg3 = reg(f, ic -> op2);
					fprintf(f, " add %s, %s, %s\n", reg1, reg2, reg3);
				}
				fprintf(f, " sw %s, %d($fp)\n", reg1, ic -> result -> offset);
				break;
			case IC_SUB:
				if (ic -> op2 -> type == OP_CONST)
				{
					reg1 = reg(f, ic -> result);
					reg2 = reg(f, ic -> op1);
					fprintf(f, " addi %s, %s, %d\n", reg1, reg2, -ic -> op2 -> value);
				}
				else
				{
					reg1 = reg(f, ic -> result);
					reg2 = reg(f, ic -> op1);
					reg3 = reg(f, ic -> op2);
					fprintf(f, " sub %s, %s, %s\n", reg1, reg2, reg3);
				}
				fprintf(f, " sw %s, %d($fp)\n", reg1, ic -> result -> offset);
				break;
			case IC_MUL:
				reg1 = reg(f, ic -> result);
				reg2 = reg(f, ic -> op1);
				reg3 = reg(f, ic -> op2);
				fprintf(f, " mul %s, %s, %s\n", reg1, reg2, reg3);
				fprintf(f, " sw %s, %d($fp)\n", reg1, ic -> result -> offset);
				break;
			case IC_DIV:
				reg1 = reg(f, ic -> result);
				reg2 = reg(f, ic -> op1);
				reg3 = reg(f, ic -> op2);
				fprintf(f, " div %s, %s\n mflo %s\n", reg2, reg3, reg1);
				fprintf(f, " sw %s, %d($fp)\n", reg1, ic -> result -> offset);
				break;
			case IC_GOTO:
				fprintf(f, " j %s\n", operand_name(ic -> unary));
				break;
			case IC_IF:
				reg1 = reg(f, ic -> src1);
				reg2 = reg(f, ic -> src2);
				switch (ic -> reltype)
				{
					case RELOP_EQ:
						fprintf(f, " beq ");
						break;
					case RELOP_NE:
						fprintf(f, " bne ");
						break;
					case RELOP_GT:
						fprintf(f, " bgt ");
						break;
					case RELOP_LT:
						fprintf(f, " blt ");
						break;
					case RELOP_GE:
						fprintf(f, " bge ");
						break;
					case RELOP_LE:
						fprintf(f, " ble ");
						break;
				}
				fprintf(f, "%s, %s, %s\n", reg1, reg2, operand_name(ic -> dest));
				break;
			case IC_RETURN:
				reg1 = reg(f, ic -> unary);
				fprintf(f, " lw $fp, %d($sp)\n", size - 4);
				fprintf(f, " addi $sp, $sp, %d\n", size);
				fprintf(f, " move $v0, %s\n jr $ra\n", reg1);
				break;
			case IC_DEC:
				break;
			case IC_ARG:
				/*
				reg1 = reg(f, ic -> unary);
				if (num_arg < 4)
				{
					fprintf(f, " move $a%d, %s\n", num_arg, reg1);
				}
				else
				{
					fprintf(f, " sw %s, %d($sp)\n", reg1, 4 * (num_arg - 4));
				}
				num_arg ++;
				*/
				num_arg ++;
				break;
			case IC_CALL:
				icl1 = icl -> prev;
				i = 0;
				if (num_arg > 4)
				{
					fprintf(f, " addi $sp, $sp, %d\n", -4 * (num_arg - 4));
				}
				while (icl1 -> ic -> type == IC_ARG)
				{
					reg1 = reg(f, icl1 -> ic -> unary);
					if (i < 4)
					{
						fprintf(f, " move $a%d, %s\n", i, reg1);
					}
					else
					{
						fprintf(f, " sw %s, %d($sp)\n", reg1, 4 * (i - 3));
					}
					icl1 = icl1 -> prev;
					i ++;
				}
				fprintf(f, " addi $sp, $sp, -4\n");
				fprintf(f, " sw $ra, 0($sp)\n");
				fprintf(f, " jal %s\n", ic -> func);
				fprintf(f, " lw $ra, 0($sp)\n");
				fprintf(f, " addi $sp, $sp, 4\n");
				if (num_arg > 4)
				{
					fprintf(f, " addi $sp, $sp, %d\n", 4 * (num_arg - 4));
				}
				if (ic -> rv != NULL)
				{
					fprintf(f, " sw $v0, %d($fp)\n", ic -> rv -> offset);
				}
				num_arg = 0;
				break;
			case IC_PARAM:
				if (num_param < 4)
				{
					fprintf(f, " sw $a%d, %d($fp)\n", num_param, ic -> unary -> offset);
				}
				num_param ++;
				break;
			case IC_READ:
				fprintf(f, " addi $sp, $sp, -4\n");
				fprintf(f, " sw $ra, 0($sp)\n");
				fprintf(f, " jal read\n");
				fprintf(f, " lw $ra, 0($sp)\n");
				fprintf(f, " addi $sp, $sp, 4\n");
				fprintf(f, " sw $v0, %d($fp)\n", ic -> unary -> offset);
				break;
			case IC_WRITE:
				if (ic -> unary -> type == OP_CONST)
				{
					fprintf(f, " li $a0, %d\n", ic -> unary -> value);
				}
				else
				{
					fprintf(f, " lw $a0, %d($fp)\n", ic -> unary -> offset);
				}
				fprintf(f, " addi $sp, $sp, -4\n");
				fprintf(f, " sw $ra, 0($sp)\n");
				fprintf(f, " jal write\n");
				fprintf(f, " lw $ra, 0($sp)\n");
				fprintf(f, " addi $sp, $sp, 4\n");
				break;
		}
		icl = icl -> next;
	} while (icl != head);
}
