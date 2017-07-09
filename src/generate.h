#ifndef _generate_h_
#define _generate_h_

#include <stdio.h>
#include "InterCode.h"

void proc_icl(InterCodeLink *icl);
void print_icl2code(FILE *f, InterCodeLink *icl);

#endif
