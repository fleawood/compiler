#include <stdio.h>
#include "Pointer.h"
extern int yylineno;
extern void yyrestart(FILE* input_file);
extern int yyparse();
struct Pointer* root;
int err_occur;
void yyerror(const char *msg)
{
	fprintf(stderr, "Error Type B at Line %d: %s\n", yylineno, msg);
}
int main(int argc, char **argv)
{
	if (argc <= 1) return 1;
	for (int i = 1; i < argc; ++i) {
		FILE *f = fopen(argv[i], "r");
		if (!f) {
			perror(argv[i]);
			return 1;
		}
		yyrestart(f);
		err_occur = 0;
		yyparse();
		if (!err_occur) {
			print_syntax_node(root, 0);
		}
		fclose(f);
	}
	return 0;
}
