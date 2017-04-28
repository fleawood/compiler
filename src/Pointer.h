#ifndef _Pointer_h_
#define _Pointer_h_
#include <stdarg.h>
enum {
	SYNTAX_TOKEN_NODE,
	LEXICAL_ID_NODE,
	LEXICAL_TYPE_NODE,
	LEXICAL_INT_NODE,
	LEXICAL_FLOAT_NODE,
	LEXICAL_OTHER_NODE
};
struct Pointer {
	int node_type;
	struct Pointer **children;
	int child_size;
	int child_cnt;
	char *name;
	int lineno;
	union {
		char * type_name;
		char * id_name;
		int ivalue;
		float fvalue;
	};
};
void insert(struct Pointer* parent, struct Pointer* child);
void insertnv(struct Pointer* parent, int n, va_list ap);
void insertn(struct Pointer* parent, int n, ...);
struct Pointer* make_node(int node_type, char* name, int lineno, char* extra);
struct Pointer* make_root(int node_type, char* name, int lineno, char* extra, int n, ...);
void print_syntax_node(struct Pointer* parent, int depth);
#endif

