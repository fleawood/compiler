#include <stdlib.h>
#include <assert.h>
#include "Type.h"
#include "link.h"

Type _type_int = {._type = TYPE_INT, .size = 4};
Type _type_float = {._type = TYPE_FLOAT, .size = 4};

ExpType _err_exp_type = {.type = NULL};

bool is_type_equal(Type *type1, Type *type2)
{
	if (type1 == NULL || type2 == NULL) return false;
	if (type1 -> _type != type2 -> _type) return false;
	Field *field1, *field2;
	switch (type1 -> _type) {
		case TYPE_INT:
		case TYPE_FLOAT:
			return true;
			break;
		case TYPE_ARRAY:
			return type1 -> _array -> length == type2 -> _array -> length && is_type_equal(type1 -> _array -> elem, type2 -> _array -> elem);
			break;
		case TYPE_STRUCT:
			field1 = type1 -> _field;
			field2 = type2 -> _field;
			while (field1 != NULL && field2 != NULL) {
				if (!is_type_equal(field1 -> symbol -> type, field2 -> symbol -> type)) {
					return false;
				}
				field1 = field1 -> next_field;
				field2 = field2 -> next_field;
			}
			if (field1 == NULL && field2 == NULL) return true;
			else return false;
			break;
		default:
			assert(0);
			break;
	}
}

Field* make_field(Symbol *symbol)
{
	Field *field = malloc(sizeof(Field));
	field -> symbol = symbol;
	field -> next_field = NULL;
	field -> offset = 0;
	return field;
}

Type* make_struct_type()
{
	Type *type = malloc(sizeof(Type));
	type -> _type = TYPE_STRUCT;
	type -> _field = NULL;
	type -> size = 0;
	return type;
}

Type* make_array_type(Type *elem, int length)
{
	Type *type = malloc(sizeof(Type));
	type -> _type = TYPE_ARRAY;
	type -> _array = malloc(sizeof(Array));
	type -> _array -> elem = elem;
	type -> _array -> length = length;
	type -> size = elem -> size * length;
	return type;
}

ExpType* make_exp_type(Type *type, bool is_lvalue)
{
	ExpType *exp = malloc(sizeof(ExpType));
	exp -> type = type;
	exp -> is_lvalue = is_lvalue;
	return exp;
}

TypeLink* make_type_link(Type *type)
{
	TypeLink *type_link = malloc(sizeof(TypeLink));
	type_link -> type = type;
	type_link -> prev = type_link;
	type_link -> next = type_link;
	return type_link;
}


void insert_field(Type *type, Field *field)
{
	insert_to_link(field, type -> _field, next_field);
}

void delete_array(Array *array)
{
	if (array == NULL) return;
	delete_type(array -> elem);
	free(array);
}

void delete_field(Field *field)
{
	if (field == NULL) return;
	delete_symbol(field -> symbol);
	delete_field(field -> next_field);
	free(field);
}

void delete_type(Type *type)
{
	if (type == NULL) return;
	switch (type -> _type) {
		case TYPE_INT:
		case TYPE_FLOAT:
			break;
		case TYPE_ARRAY:
			delete_array(type -> _array);
			free(type);
			break;
		case TYPE_STRUCT:
			delete_field(type -> _field);
			free(type);
			break;
		default:
			assert(0);
			break;
	}
}
