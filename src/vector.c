#include <stdlib.h>
#include <assert.h>
#include "vector.h"

vector* vector_init()
{
	vector *v = malloc(sizeof(vector));
	assert(v != NULL);
	v -> data = malloc(sizeof(void*));
	assert(v -> data != NULL);
	v -> cap = 1;
	v -> size = 0;
	return v;
}

void vector_insert(vector *v, void *x)
{
	if (v -> cap == v -> size) {
		v -> cap *= 2;
		void *data = realloc(v -> data, sizeof(void *) * v -> cap);
		assert(data != NULL);
		v -> data = data;
	}
	v -> data[v -> size] = x;
	v -> size ++;
}

