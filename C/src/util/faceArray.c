#include <stdlib.h>
#include "faceArray.h"

int faceArray_init(faceArray* arr, size_t initialSize){

	if (arr == NULL || initialSize <= 0)
		return 1;
	
	arr->maxSize = initialSize;
	arr->size = 0;
	arr->faces = malloc(initialSize * sizeof(vector3lArray));
	return -(arr->faces == NULL); // 0 success, -1 failure
}

int faceArray_append(faceArray* arr, vector3lArray face){
	
	if (arr->maxSize <= arr->size){
		
		arr->maxSize <<= 1;
		vector3lArray* newSpace = realloc(arr->faces, arr->maxSize * sizeof(vector3lArray));
		if (newSpace == NULL){
			arr->maxSize >>= 1;
			return -1;
		}
		arr->faces = newSpace;
	}
	
	arr->faces[arr->size++] = face;
	
	return 0;
}

vector3lArray faceArray_get(faceArray* arr, size_t pos){
	if (pos >= arr->size){
		vector3lArray res;
		res.maxSize = res.size = 0;
		res.values = NULL;
		return res;
	}
	return arr->faces[pos];
}

void faceArray_free(faceArray* arr){
	for (int i = 0; i < arr->size; ++i)
		vector3lArray_free(arr->faces + i);
	free(arr->faces);
}
