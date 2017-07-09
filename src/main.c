#include <stdio.h>
#include "Node.h"
#include "parse.h"
#include "InterCode.h"
#include "generate.h"

extern void yyrestart(FILE* input_file);
extern int yyparse();
Node* root;
int err_occur;

int main(int argc, char **argv)
{
	if (argc != 3) return 1;
	FILE *fin = fopen(argv[1], "r");
	if (!fin) {
		perror(argv[1]);
		return 1;
	}
	yyrestart(fin);
	err_occur = 0;
	yyparse();
	if (!err_occur) {
		FILE *fout = fopen(argv[2], "w");
		InterCodeLink* icl = parse_syntax(root);
		//print_icl(fout, icl);
		proc_icl(icl);
		print_icl2code(fout, icl);
		fclose(fout);
	}
	fclose(fin);
	return 0;
}
