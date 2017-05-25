#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "Symbol.h"
#include "hash.h"
#include "link.h"
#include "misc.h"

SymbolLink *hash_table[HASH_SIZE];

FuncInfo* make_func_info(Type *type)
{
	FuncInfo *func = malloc(sizeof(FuncInfo));
	func -> type = type;
	func -> args = NULL;
	return func;
}

ArgLink *make_arg_link(Symbol *symbol)
{
	ArgLink* arg_link = malloc(sizeof(ArgLink));
	arg_link -> arg = symbol;
	arg_link -> next = NULL;
	return arg_link;
}

Symbol* make_var_symbol(Type *type, const char *name, int lineno)
{
	Symbol *symbol = malloc(sizeof(Symbol));
	symbol -> type = type;
	symbol -> name = (char *)name;
	symbol -> lineno = lineno;
	symbol -> is_func = false;
	return symbol;
}

char *make_struct_name(const char *name)
{
	int n = strlen(name);
	char *_name = malloc(n+2);
	*_name = '#';
	strcpy(_name + 1, name);
	return _name;
}

Symbol* make_struct_symbol(const char *name, int lineno)
{
	Type *type = make_struct_type();
	return make_var_symbol(type, make_struct_name(name), lineno);
}

Symbol* make_array_symbol(Type *type, const char *name, int lineno)
{
//	Type *type = make_array_type(elem, size);
	return make_var_symbol(type, name, lineno);
}

Symbol* make_func_symbol(FuncInfo *func, const char *name, int lineno)
{
	Symbol *symbol = malloc(sizeof(Symbol));
	symbol -> func = func;
	symbol -> name = (char *)name;
	symbol -> lineno = lineno;
	symbol -> is_func = true;
	return symbol;
}

SymbolLink* make_symbol_link(Symbol *symbol, Stack *stack)
{
	SymbolLink *symbol_link = malloc(sizeof(SymbolLink));
	symbol_link -> symbol = symbol;
	symbol_link -> stack = stack;
	symbol_link -> next_stack_link = NULL;
	symbol_link -> prev_hash_link = NULL;
	symbol_link -> next_hash_link = NULL;
	return symbol_link;
}

void insert_symbol_head(SymbolLink *symbol_link, Stack *stack)
{
	insert_to_link(symbol_link, stack -> symbol_head, next_stack_link);
}

void insert_hash_table(SymbolLink *symbol_link, SymbolLink *entry)
{
	SymbolLink *head = entry -> next_hash_link;
	symbol_link -> next_hash_link = head;
	if (head != NULL) head -> prev_hash_link = symbol_link;
	symbol_link -> prev_hash_link = entry;
	entry -> next_hash_link = symbol_link;
	//insert_to_link(symbol_link, entry -> next_hash_link, next_hash_link);
}

Field* find_field_in_struct(const char *name, Type *type)
{
	for (Field *p = type -> _field; p != NULL; p = p -> next_field) {
		char *_name = p -> symbol -> name;
		if (strcmp(name, _name) == 0) return p;
	}
	return NULL;
}

Symbol *find_symbol_in_stack(const char *name, Stack *stack)
{
	uint32_t val = hash(name);
	for (SymbolLink *p = hash_head(val); p != NULL; p = p -> next_hash_link) {
		char *_name = p -> symbol -> name;
		Stack *_stack = p -> stack;
		if (stack == _stack && strcmp(name, _name) == 0) {
			return p -> symbol;
		}
	}
	return NULL;
}

Symbol *find_symbol(const char *name)
{
	uint32_t val = hash(name);
	for (SymbolLink *p = hash_head(val); p != NULL; p = p -> next_hash_link) {
		char *_name = p -> symbol -> name;
		if (strcmp(name, _name) == 0) {
			return p -> symbol;
		}
	}
	return NULL;
}

Type *find_struct(const char *name)
{
	Symbol *symbol = find_symbol(make_struct_name(name));
	if (symbol == NULL || !is_type_struct(symbol -> type)) return NULL;
	return symbol -> type;
}

int check_func_args(ArgLink *arg_link, TypeLink *type_link)
{
	while (arg_link != NULL && type_link != NULL) {
		if (!is_type_equal(arg_link -> arg -> type, type_link -> type)) return -1;
		arg_link = arg_link -> next;
		type_link = type_link -> next;
	}
	if (arg_link == NULL && type_link == NULL) return 0;
	else return -1;
}

int check_func_dec_args(ArgLink *arg_link1, ArgLink *arg_link2)
{
	while (arg_link1 != NULL && arg_link2 != NULL) {
		if (!is_type_equal(arg_link1 -> arg -> type, arg_link2 -> arg -> type)) return -1;
		arg_link1 = arg_link1 -> next;
		arg_link2 = arg_link2 -> next;
	}
	if (arg_link1 == NULL && arg_link2 == NULL) return 0;
	else return -1;
}

void check_func_def()
{
	Stack *stack = stack_top;
	for (SymbolLink *p = stack -> symbol_head; p != NULL; p = p -> next_stack_link) {
		Symbol *symbol = p -> symbol;
		if (symbol -> is_func) {
			if (!symbol -> func -> is_def) {
				parse_error(ERR_FUNC_NODEF, symbol -> lineno, "incomplete definition of function \"%s\".", symbol -> name);
			}
		}
	}
}

void insert_var_to_struct(Symbol *symbol, Type *type)
{
	if (find_field_in_struct(symbol -> name, type) == NULL) {
		Field *field = make_field(symbol);
		insert_to_link(field, type -> _field, next_field);
	} else {
		parse_error(ERR_FIELD_REDEF, symbol -> lineno, "redefined field \"%s\".", symbol -> name);
	}
}

static void insert_symbol_to_stack(Symbol *symbol, Stack *stack)
{
	SymbolLink *symbol_link = make_symbol_link(symbol, stack);
	insert_symbol_head(symbol_link, stack_top);
	insert_hash_table(symbol_link, hash_table[hash(symbol -> name)]);
}

void insert_var_to_stack(Symbol *symbol, Stack *stack)
{
	if (find_symbol_in_stack(symbol -> name, stack) == NULL) {
		insert_symbol_to_stack(symbol, stack);
	} else {
		parse_error(ERR_VAR_REDEF, symbol -> lineno, "redefined variable \"%s\".", symbol -> name);
	}
}

void insert_func_def_to_stack(Symbol *symbol, Stack *stack)
{
	Symbol *_symbol = find_symbol(symbol -> name);
	if (_symbol == NULL) {
		insert_symbol_to_stack(symbol, stack);
	} else if (_symbol -> is_func) {
		if (_symbol -> func -> is_def) {
			parse_error(ERR_FUNC_REDEF, symbol -> lineno, "redefined function \"%s\".", symbol -> name);
		} else {
			if (check_func_dec_args(symbol -> func -> args, _symbol -> func -> args) != 0) {
				parse_error(ERR_FUNC_DEC, symbol -> lineno, "incompatible function declaration.");
			}
			_symbol -> func -> is_def = true;
		}
	} else {
		parse_error(ERR_FUNC_REDEF, symbol -> lineno, "redefined function \"%s\".", symbol -> name);
	}
}

void insert_func_dec_to_stack(Symbol *symbol, Stack *stack)
{
	Symbol *_symbol = find_symbol(symbol -> name);
	if (_symbol == NULL) {
		insert_symbol_to_stack(symbol, stack);
	} else if (_symbol -> is_func) {
		if (check_func_dec_args(symbol -> func -> args, _symbol -> func -> args) != 0) {
			parse_error(ERR_FUNC_DEC, symbol -> lineno, "incompatible function declaration.");
		}
	} else {
		parse_error(ERR_FUNC_REDEF, symbol -> lineno, "redefined function \"%s\".", symbol -> name);
	}
}

void insert_var_to_func_args(Symbol *symbol, FuncInfo *func)
{
	ArgLink *arg_link = make_arg_link(symbol);
	insert_to_link(arg_link, func -> args, next);
}

void insert_func_args_to_stack(FuncInfo *func, Stack *stack)
{
	for (ArgLink *args = func -> args; args != NULL; args = args -> next) {
		Symbol *symbol = args -> arg;
		insert_var_to_stack(symbol, stack);
	}
}

void insert_struct_to_stack(Symbol *symbol, Stack *stack)
{
	if (find_symbol_in_stack(symbol -> name, stack) == NULL) {
		insert_symbol_to_stack(symbol, stack);
	} else {
		parse_error(ERR_STRUCT_REDEF, symbol -> lineno, "redefined struct \"%s\".", symbol -> name + 1);
	}

}

void delete_symbol(Symbol *symbol)
{
	if (symbol == NULL) return;
	free(symbol);
}

void delete_symbol_link(SymbolLink *symbol_link)
{
	if (symbol_link == NULL) return;
	delete_symbol(symbol_link -> symbol);
	free(symbol_link);
}

void delete_symbol_on_stack(Stack *stack)
{
	for (SymbolLink *p = stack -> symbol_head, *nextp; p != NULL; p = nextp) {
		SymbolLink *prev = p -> prev_hash_link;
		SymbolLink *next = p -> next_hash_link;
		if (prev != NULL) prev -> next_hash_link = next;
		if (next != NULL) next -> prev_hash_link = prev;
		nextp = p -> next_stack_link;
		delete_symbol_link(p);
	}
}

void hash_table_init()
{
	for (int i = 0; i < HASH_SIZE; ++i) {
		SymbolLink *symbol_link = make_symbol_link(NULL, NULL);
		hash_table[i] = symbol_link;
	}
}
