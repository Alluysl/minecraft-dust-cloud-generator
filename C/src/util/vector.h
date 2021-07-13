#include <stddef.h>

#ifndef vector3_h_included
#define vector3_h_included

/* vector3f */

typedef struct {
	float x, y, z;
} vector3f;

vector3f vector3f_of(float x, float y, float z);
vector3f vector3f_zero();
vector3f vector3f_add(vector3f a, vector3f b);
vector3f vector3f_subtract(vector3f subtractee, vector3f subtractor);
vector3f vector3f_difference(vector3f from, vector3f to);

typedef struct {
	size_t maxSize;
	size_t size;
	vector3f* values;
} vector3fArray;

int vector3fArray_init(vector3fArray* arr, size_t initialSize); // returns 0 on success
int vector3fArray_append(vector3fArray* arr, vector3f v); // returns 0 on success
vector3f vector3fArray_get(vector3fArray* arr, size_t pos); // returns a copy, not a reference; (0,0,0) on wrong index
int vector3fArray_set(vector3fArray* arr, size_t pos, vector3f v); // returns 0 on success
void vector3fArray_free(vector3fArray* arr);

/* vector3l */

typedef struct {
	long x, y, z;
} vector3l;

vector3l vector3l_of(long x, long y, long z);
vector3l vector3l_zero();

typedef struct {
	size_t maxSize;
	size_t size;
	vector3l* values;
} vector3lArray;

int vector3lArray_init(vector3lArray* arr, size_t initialSize); // returns 0 on success
int vector3lArray_append(vector3lArray* arr, vector3l v); // returns 0 on success
vector3l vector3lArray_get(vector3lArray* arr, size_t pos); // returns a copy, not a reference; (0,0,0) on wrong index
int vector3lArray_set(vector3lArray* arr, size_t pos, vector3l v); // returns 0 on success
void vector3lArray_free(vector3lArray* arr);

/* vector4f */

typedef struct {
	float x, y, z, w;
} vector4f;

vector4f vector4f_of(float x, float y, float z, float w);
vector4f vector4f_zero();

#endif /* ifndef vector3_h_included */
