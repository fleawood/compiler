#include <stdio.h>
#include "misc.h"
extern int yylineno;
void yyerror(const char *msg)
{
	fprintf(stderr, "Error Type B at Line %d: %s\n", yylineno, msg);
}

