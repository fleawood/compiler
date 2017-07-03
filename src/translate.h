#ifndef _translate_h_
#define _translate_h_

#include "InterCode.h"
#include "Operand.h"
#include "Node.h"
#include "Symbol.h"

InterCodeLink* translate_Addr(Node *p, Type **type, Operand **op);
InterCodeLink* translate_Exp(Node *p, Operand **op);
InterCodeLink* translate_Cond(Node *p, Operand *label_true, Operand *label_false);
//InterCodeLink* translate_Stmt(Node *p);
InterCodeLink* translate_Args(Node *p, OperandLink **args);
//InterCodeLink* translate_CompSt(Node *p);
InterCodeLink* translate_FunDec(Node *p, FuncInfo *func);
InterCodeLink* translate_VarDec(Node *p, Type *type);

#endif
