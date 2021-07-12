#include "vector3.h"

#ifndef obj_h_included
#define obj_h_included

#define OBJ_FIELD_ARRAY_START_SIZE 16

typedef struct {
	
	vector3fArray vertices;
	vector3fArray uv;
	vector3lArray faceVertices;
	
} objData;

typedef enum { OBJ_LP_ERROR = -1, OBJ_LP_SUCCESS = 0 /* set explicitly for clarity */, OBJ_LP_SKIPPED, OBJ_LP_INVALID } objLineParsingResult;

int objData_init(objData* data);
objLineParsingResult objData_parse_line(char* line, objData* data);
void objData_free(objData* data);

#endif /* #ifndef obj_h_included */
