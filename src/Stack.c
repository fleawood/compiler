#include <stdlib.h>
#include "Stack.h"

Stack *stack_top;

Stack* make_stack(Stack *next_stack)
{
	Stack *stack = malloc(sizeof(Stack));
	stack -> next_stack = next_stack;
	stack -> symbol_head = NULL;
	return stack;
}

void delete_stack(Stack *stack)
{
	delete_symbol_on_stack(stack);
	free(stack);
}

void push_stack()
{
	Stack *stack = make_stack(stack_top);
	stack_top = stack;
}

void pop_stack()
{
	Stack *stack = stack_top;
	stack_top = stack_top -> next_stack;
	delete_stack(stack);
}
