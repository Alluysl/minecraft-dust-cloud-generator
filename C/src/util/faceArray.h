#include "vector.h"

#ifndef faceArray_h_included
#define faceArray_h_included

typedef struct {
	size_t maxSize;
	size_t size;
	vector3lArray* faces;
} faceArray;

int faceArray_init(faceArray* arr, size_t initialSize);
int faceArray_append(faceArray* arr, vector3lArray face);
vector3lArray faceArray_get(faceArray* arr, size_t pos);
void faceArray_free(faceArray* arr);

#endif /* #ifndef faceArray_h_included */
