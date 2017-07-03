#ifndef _Symbol_h_
#define _Symbol_h_

#include <stdbool.h>

typedef struct Symbol Symbol;
typedef struct SymbolLink SymbolLink;
typedef struct FuncInfo FuncInfo;
typedef struct ArgLink ArgLink;

#include "Type.h"
#include "Stack.h"
#include "Operand.h"

struct FuncInfo
{
	Type *type;
	bool is_def;
	int argc;
	struct ArgLink {
		Symbol *arg;
		ArgLink *next;
	} *args;
};

struct Symbol
{
	char *name;
	int lineno;
	bool is_func;
	Operand *op;
	union {
		Type *type;
		FuncInfo *func;
	};
};

struct SymbolLink
{
	Symbol *symbol;
	Stack *stack;
	SymbolLink *next_stack_link;
	SymbolLink *prev_hash_link;
	SymbolLink *next_hash_link;
};

FuncInfo *make_func_info(Type *type);
ArgLink *make_arg_link(Symbol *symbol);

Symbol* make_var_symbol(Type *type, const char *name, int lineno);
Symbol* make_struct_symbol(const char *name, int lineno);
Symbol* make_array_symbol(Type *type, const char *name, int lineno);
Symbol* make_func_symbol(FuncInfo *func, const char *name, int lineno);

SymbolLink* make_symbol_link(Symbol *symbol, Stack *stack);

Field *find_field_in_struct(const char *name, Type *type);
Symbol *find_symbol_in_stack(const char *name, Stack *stack);
Symbol *find_symbol(const char *name);
Type* find_struct(const char *name);

int check_func_args(ArgLink *arg_link, TypeLink *type_link);
int check_func_dec_args(ArgLink *arg_link1, ArgLink *arg_link2);
void check_func_def();

void insert_var_to_struct(Symbol *symbol, Type *type);
void insert_var_to_stack(Symbol *symbol, Stack *stack);
void insert_func_def_to_stack(Symbol *symbol, Stack *stack);
void insert_func_dec_to_stack(Symbol *symbol, Stack *stack);
void insert_var_to_func_args(Symbol *symbol, FuncInfo *func);
void insert_func_args_to_stack(FuncInfo *func, Stack *stack);
void insert_struct_to_stack(Symbol *symbol, Stack *stack);

void delete_symbol(Symbol *symbol);
void delete_symbol_link(SymbolLink *symbolLink);
void delete_symbol_on_stack(Stack *stack);

#define HASH_SIZE 0x3fff
#define hash_head(val) (hash_table[val] -> next_hash_link)
void hash_table_init();

#endif
