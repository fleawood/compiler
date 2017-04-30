#ifndef _vector_h_
#define _vector_h_

typedef struct vector vector;

struct vector
{
	void **data;
	int cap;
	int size;
};

vector* vector_init();
#define new_vector() vector_init()

void vector_insert(vector *v, void *x);

#endif
