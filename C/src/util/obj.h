#include "faceArray.h"
#include "vector.h"

#ifndef obj_h_included
#define obj_h_included

#define OBJ_FIELD_ARRAY_START_SIZE 16

typedef struct {
	
	vector3fArray vertices;
	vector3fArray uv;
	faceArray faces;
	vector4f* vertexColors;
	
} objData;

typedef enum { OBJ_LR_ERROR = -1, OBJ_LR_SUCCESS = 0 /* set explicitly for clarity */, OBJ_LR_EOF, OBJ_LR_EMPTY } objLineReadingResult;
typedef enum { OBJ_LP_ERROR = -1, OBJ_LP_SUCCESS = 0 /* set explicitly for clarity */, OBJ_LP_SKIPPED, OBJ_LP_INVALID } objLineParsingResult;

objData* objData_load_from_file(char* path, char* texturePath, double pixelFloatPrecision);
char* objData_get_error(int* isSystem);
void objData_free(objData* data);

#endif /* #ifndef obj_h_included */
