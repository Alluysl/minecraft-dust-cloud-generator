#include <stdlib.h>
#include <math.h>
#include "vector.h"

/* vector3f */

vector3f vector3f_of(float x, float y, float z){
	vector3f res = { x, y, z };
	return res;
}

vector3f vector3f_zero(){
	vector3f res = { 0, 0, 0 };
	return res;
}

vector3f vector3f_add(vector3f a, vector3f b){
	
	vector3f res = { a.x + b.x, a.y + b.y, a.z + b.z };
	return res;
}

vector3f vector3f_subtract(vector3f subtractee, vector3f subtractor){
	
	vector3f res = {
			subtractee.x - subtractor.x,
			subtractee.y - subtractor.y,
			subtractee.z - subtractor.z
		};
	return res;
}

vector3f vector3f_difference(vector3f from, vector3f to){
	return vector3f_subtract(to, from);
}

int vector3fArray_init(vector3fArray* arr, size_t initialSize){

	if (arr == NULL || initialSize <= 0)
		return 1;
	
	arr->maxSize = initialSize;
	arr->size = 0;
	arr->values = malloc(initialSize * sizeof(vector3f));
	return -(arr->values == NULL); // 0 success, -1 failure
}

int vector3fArray_append(vector3fArray* arr, vector3f v){
	
	if (arr->maxSize <= arr->size){
		
		arr->maxSize <<= 1;
		vector3f* newSpace = realloc(arr->values, arr->maxSize * sizeof(vector3f));
		if (newSpace == NULL){
			arr->maxSize >>= 1;
			return -1;
		}
		arr->values = newSpace;
	}
	
	arr->values[arr->size++] = v;
	
	return 0;
}

vector3f vector3fArray_get(vector3fArray* arr, size_t pos){
	if (pos >= arr->size)
		return vector3f_of(0, 0, 0);
	return arr->values[pos];
}

int vector3fArray_set(vector3fArray* arr, size_t pos, vector3f v){
	if (pos >= arr->size)
		return 1;
	arr->values[pos] = v;
	return 0;
}

void vector3fArray_free(vector3fArray* arr){
	free(arr->values);
}

/* vector3l */

vector3l vector3l_of(long x, long y, long z){
	vector3l res = { x, y, z };
	return res;
}

vector3l vector3l_zero(){
	vector3l res = { 0, 0, 0 };
	return res;
}

int vector3lArray_init(vector3lArray* arr, size_t initialSize){

	if (arr == NULL || initialSize <= 0)
		return 1;
	
	arr->maxSize = initialSize;
	arr->size = 0;
	arr->values = malloc(initialSize * sizeof(vector3l));
	return -(arr->values == NULL); // 0 success, -1 failure
}

int vector3lArray_append(vector3lArray* arr, vector3l v){
	
	if (arr->maxSize <= arr->size){
		
		arr->maxSize <<= 1;
		vector3l* newSpace = realloc(arr->values, arr->maxSize * sizeof(vector3l));
		if (newSpace == NULL){
			arr->maxSize >>= 1;
			return -1;
		}
		arr->values = newSpace;
	}
	
	arr->values[arr->size++] = v;
	
	return 0;
}

vector3l vector3lArray_get(vector3lArray* arr, size_t pos){
	if (pos >= arr->size)
		return vector3l_of(0, 0, 0);
	return arr->values[pos];
}

int vector3lArray_set(vector3lArray* arr, size_t pos, vector3l v){
	if (pos >= arr->size)
		return 1;
	arr->values[pos] = v;
	return 0;
}

void vector3lArray_free(vector3lArray* arr){
	free(arr->values);
}

/* vector4f */

vector4f vector4f_of(float x, float y, float z, float w){
	vector4f res = { x, y, z, w };
	return res;
}

vector4f vector4f_zero(){
	vector4f res = { 0, 0, 0, 0 };
	return res;
}
