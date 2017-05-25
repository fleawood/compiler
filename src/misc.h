#ifndef _misc_h_
#define _misc_h_

#include <stdio.h>

void yyerror(const char *msg);

#define ERR_VAR_UNDEF       1
#define ERR_FUNC_UNDEF      2
#define ERR_VAR_REDEF       3
#define ERR_FUNC_REDEF      4
#define ERR_INCOMP_ASSIGN   5
#define ERR_RVALUE_ASSIGN   6
#define ERR_INVALID_OPERAND 7
#define ERR_RETURN_TYPE     8
#define ERR_FUNC_ARGS       9
#define ERR_ARRAY_ACCESS    10
#define ERR_FUNC_ACCESS     11
#define ERR_ARRAY_SUBSCRIPT 12
#define ERR_FIELD_ACCESS    13
#define ERR_FIELD_UNDEF     14
#define ERR_FIELD_REDEF     15
#define ERR_FIELD_INIT      15
#define ERR_STRUCT_REDEF    16
#define ERR_STRUCT_UNDEF    17
#define ERR_FUNC_NODEF      18
#define ERR_FUNC_DEC        19

#define parse_error(err_type, lineno, msg, ...) { \
	fprintf(stderr, "Error Type %d at Line %d: " msg "\n", err_type, lineno, ##__VA_ARGS__); \
}

#endif
