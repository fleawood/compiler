#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "Pointer.h"

void insert(struct Pointer * parent, struct Pointer * child)
{
	if (parent -> child_cnt == parent -> child_size) {
		parent -> child_size *= 2;
		void *new_children = realloc(parent -> children, (sizeof (struct Pointer*)) * parent -> child_size);
		if (!new_children) {
			fprintf(stderr, "realloc error!\n");
		}
		parent -> children = new_children;
	}
	parent -> children[parent -> child_cnt] = child;
	parent -> child_cnt ++;
}
void insertnv(struct Pointer* parent, int n, va_list ap)
{
	int i;
	for (i = 0; i < n; ++i) {
		struct Pointer *child = va_arg(ap, struct Pointer*);
		insert(parent, child);
	}
}
void insertn(struct Pointer * parent, int n, ...)
{
	va_list ap;
	va_start(ap, n);
	insertnv(parent, n, ap);
	va_end(ap);
}
struct Pointer* make_node(int node_type, char *name, int lineno, char *extra)
{
	struct Pointer *p = malloc(sizeof (struct Pointer));
	if (p == NULL) {
		printf("ERROR\n");
	}
	p -> children = malloc(sizeof (struct Pointer *));
	p -> child_size = 1;
	p -> child_cnt = 0;
	p -> lineno = lineno;
	p -> node_type = node_type;
	p -> name = malloc((sizeof (char)) * (strlen(name) + 1));
	strcpy(p -> name, name);
	switch (p -> node_type) {
		case SYNTAX_TOKEN_NODE:
			break;
		case LEXICAL_ID_NODE:
			p -> id_name = malloc((sizeof (char)) * (strlen(extra) + 1));
			strcpy(p -> id_name, extra);
			break;
		case LEXICAL_TYPE_NODE:
			p -> type_name = malloc((sizeof (char)) * (strlen(extra) + 1));
			strcpy(p -> type_name, extra);
			break;
		case LEXICAL_INT_NODE:
			p -> ivalue = strtol(extra, NULL, 0);
			break;
		case LEXICAL_FLOAT_NODE:
			p -> fvalue = strtof(extra, NULL);
			break;
		case LEXICAL_OTHER_NODE:
			break;
		default:
			break;
	}
	return p;
}
struct Pointer* make_root(int node_type, char* name, int lineno, char* extra, int n, ...)
{
	struct Pointer *parent = make_node(node_type, name, lineno, extra);
	va_list ap;
	va_start(ap, n);
	insertnv(parent, n, ap);
	va_end(ap);
	return parent;
}
void print_syntax_node(struct Pointer* parent, int depth)
{
	if (parent == NULL) return;
	int i;
	for (i = 0; i < depth; ++i) putchar(' ');
	printf("%s", parent -> name);
	switch (parent -> node_type) {
		case SYNTAX_TOKEN_NODE:
			printf(" (%d)", parent -> lineno);
			break;
		case LEXICAL_ID_NODE:
			printf(": %s", parent -> id_name);
			break;
		case LEXICAL_TYPE_NODE:
			printf(": %s", parent -> type_name);
			break;
		case LEXICAL_INT_NODE:
			printf(": %d", parent -> ivalue);
			break;
		case LEXICAL_FLOAT_NODE:
			printf(": %f", parent -> fvalue);
			break;
	}
	puts("");
	for (i = 0; i < parent -> child_cnt; ++i) {
		print_syntax_node(parent -> children[i], depth + 2);
	}
}
