#ifndef _Node_h_
#define _Node_h_
#include <stdarg.h>
#include "token.h"
#include "vector.h"

enum {
	SYNTAX_TOKEN,
	LEXICAL_ID,
	LEXICAL_TYPE,
	LEXICAL_INT,
	LEXICAL_FLOAT,
	LEXICAL_OTHER
};

typedef struct Node Node;

struct Node {
	int node_type;
	int print_type;
	vector *child;
	Node *parent;
	int lineno;
	union {
		char *svalue;
		int ivalue;
		float fvalue;
	};
};

#define child(p, n) ((Node **)(p -> child -> data))[n]
#define child_type(p, n) (child(p, n) == NULL ? _NULL : child(p, n) -> node_type)
#define child_cnt(p) (p -> child -> size)

Node* make_leaf(int node_type, int print_type, int lineno, char* extra);
Node* make_node(int node_type, int print_type, int lineno, char* extra, int n, ...);
void print_syntax_node(Node *p, int depth);

#endif

