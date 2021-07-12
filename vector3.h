#include <stddef.h>

#ifndef vector3_h_included
#define vector3_h_included

typedef struct {
	float x, y, z;
} vector3f;

typedef struct {
	size_t maxSize;
	size_t size;
	vector3f* values;
} vector3fArray;

vector3f vector3f_of(float x, float y, float z);
vector3f vector3f_zero();

int vector3fArray_init(vector3fArray* arr, size_t initialSize); // returns 0 on success
int vector3fArray_append(vector3fArray* arr, vector3f v); // returns 0 on success
vector3f vector3fArray_get(vector3fArray* arr, size_t pos); // returns a copy, not a reference; (0,0,0) on wrong index
int vector3fArray_set(vector3fArray* arr, size_t pos, vector3f v); // returns 0 on success
void vector3fArray_free(vector3fArray* arr);

typedef struct {
	long x, y, z;
} vector3l;

typedef struct {
	size_t maxSize;
	size_t size;
	vector3l* values;
} vector3lArray;

vector3l vector3l_of(long x, long y, long z);
vector3l vector3l_zero();

int vector3lArray_init(vector3lArray* arr, size_t initialSize); // returns 0 on success
int vector3lArray_append(vector3lArray* arr, vector3l v); // returns 0 on success
vector3l vector3lArray_get(vector3lArray* arr, size_t pos); // returns a copy, not a reference; (0,0,0) on wrong index
int vector3lArray_set(vector3lArray* arr, size_t pos, vector3l v); // returns 0 on success
void vector3lArray_free(vector3lArray* arr);

#endif /* ifndef vector3_h_included */
