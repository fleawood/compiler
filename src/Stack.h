#ifndef _Stack_h_
#define _Stack_h_

typedef struct Stack Stack;

#include "Symbol.h"

struct Stack
{
	Stack *next_stack;
	SymbolLink *symbol_head;
};

extern Stack *stack_top;

Stack *make_stack(Stack *next_stack);
void delete_stack(Stack *stack);

void push_stack();
void pop_stack();

#endif
