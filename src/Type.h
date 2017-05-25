#ifndef _Type_h_
#define _Type_h_

typedef struct Type Type;
typedef struct Field Field;
typedef struct Array Array;
typedef struct ExpType ExpType;
typedef struct TypeLink TypeLink;

#include "Symbol.h"

struct Type
{
	enum {
		TYPE_INT,
		TYPE_FLOAT,
		TYPE_ARRAY,
		TYPE_STRUCT
	} _type;
	union {
		struct Array {
			Type *elem;
			int size;
		} *_array;
		struct Field {
			Symbol *symbol;
			Field *next_field;
		} *_field;
	};
};

struct ExpType
{
	Type *type;
	bool is_lvalue;
};

struct TypeLink
{
	Type *type;
	TypeLink *next;
};


extern Type _type_int, _type_float;

#define type_int &_type_int
#define type_float &_type_float

#define is_type_int(type) (type != NULL && type -> _type == TYPE_INT)
#define is_type_float(type) (type != NULL && type -> _type == TYPE_FLOAT)
#define is_type_array(type) (type != NULL && type -> _type == TYPE_ARRAY)
#define is_type_struct(type) (type != NULL && type -> _type == TYPE_STRUCT)
#define is_type_int_or_float(type) (is_type_int(type) || is_type_float(type))
//#define is_type_equal(type1, type2) (type1 != NULL && type1 -> _type == type2 -> _type)
bool is_type_equal(Type *type1, Type *type2);

#define elem_type(type) (type -> _array -> elem)
#define field_type(field) (field -> symbol -> type)

extern ExpType _err_exp_type;
#define err_exp_type &_err_exp_type

Field* make_field(Symbol *symbol);
Type* make_struct_type();
Type* make_array_type(Type *elem, int size);
ExpType *make_exp_type(Type *type, bool is_lvalue);
TypeLink *make_type_link(Type *type);

void delete_type(Type *type);
void delete_array(Array *array);
void delete_field(Field *field);

#endif
