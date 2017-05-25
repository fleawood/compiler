#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include "Node.h"

static void insertnv(Node *p, int n, va_list ap)
{
	for (int i = 0; i < n; ++i) {
		Node *q = va_arg(ap, Node *);
		vector_insert(p -> child, q);
		if (q != NULL) q -> parent = p;
	}
}

static void insertn(Node *p, int n, ...)
{
	va_list ap;
	va_start(ap, n);
	insertnv(p, n, ap);
	va_end(ap);
}

Node* make_leaf(int node_type, int print_type, int lineno, char *extra)
{
	Node *p = malloc(sizeof (Node));
	assert(p != NULL);

	p -> child = new_vector();
	p -> lineno = lineno;
	p -> node_type = node_type;
	p -> print_type = print_type;
	p -> parent = NULL;

	switch (p -> print_type) {
		case SYNTAX_TOKEN:
			break;
		case LEXICAL_ID:
		case LEXICAL_TYPE:
			p -> svalue = malloc(sizeof(char) * (strlen(extra)+1));
			strcpy(p -> svalue, extra);
			break;
		case LEXICAL_INT:
			p -> ivalue = strtol(extra, NULL, 0);
			break;
		case LEXICAL_FLOAT:
			p -> fvalue = strtof(extra, NULL);
			break;
		case LEXICAL_OTHER:
			break;
		default:
			break;
	}
	return p;
}

Node* make_node(int node_type, int print_type, int lineno, char* extra, int n, ...)
{
	Node *p = make_leaf(node_type, print_type, lineno, extra);
	va_list ap;
	va_start(ap, n);
	insertnv(p, n, ap);
	va_end(ap);
	return p;
}

void print_syntax_node(Node* p, int depth)
{
	if (p == NULL) return;
	for (int i = 0; i < depth; ++i) putchar(' ');
	printf("%s", token_name(p -> node_type));
	switch (p -> print_type) {
		case SYNTAX_TOKEN:
			printf(" (%d)", p -> lineno);
			break;
		case LEXICAL_ID:
		case LEXICAL_TYPE:
			printf(": %s", p -> svalue);
			break;
		case LEXICAL_INT:
			printf(": %d", p -> ivalue);
			break;
		case LEXICAL_FLOAT:
			printf(": %f", p -> fvalue);
			break;
	}
	puts("");
	for (int i = 0; i < child_cnt(p); ++i) {
		print_syntax_node(child(p, i), depth + 2);
	}
}

