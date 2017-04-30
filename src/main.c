#include <stdio.h>
#include "Node.h"
#include "parse.h"

extern void yyrestart(FILE* input_file);
extern int yyparse();
Node* root;
int err_occur;

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
			parse_syntax_node(root);
		}
		fclose(f);
	}
	return 0;
}
