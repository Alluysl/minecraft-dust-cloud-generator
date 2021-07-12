#include <stdio.h>
#include <string.h>

#include "obj.h"

int objData_init(objData* data){
	
	if (vector3fArray_init(&data->vertices, OBJ_FIELD_ARRAY_START_SIZE))
		return -1;
	if (vector3fArray_init(&data->uv, OBJ_FIELD_ARRAY_START_SIZE)){
		vector3fArray_free(&data->vertices);
		return -1;
	}
	if (vector3lArray_init(&data->faceVertices, OBJ_FIELD_ARRAY_START_SIZE)){
		vector3fArray_free(&data->vertices);
		vector3fArray_free(&data->uv);
		return -1;
	}
	
	return 0;
}

objLineParsingResult objData_parse_line(char* line, objData* data){
	
	int r;
	if (line[0] == 'v' && line[1] == ' '){
		
		vector3f v;
		r = sscanf(line + 2, "%f %f %f", &v.x, &v.y, &v.z);
		if (r != 3)
			return (r == EOF) * -1 + (r != EOF) * OBJ_LP_INVALID;  /* OBJ_LP_INVALID on missing data, -1 on error */
		return vector3fArray_append(&data->vertices, v); /* 0 on success, -1 on error */
		
	} else if (line[0] == 'v' && line[1] == 't' && line[2] == ' '){
		
		vector3f v = { 0, 0, 0 };
		r = sscanf(line + 3, "%f %f %f", &v.x, &v.y, &v.z);
		/* W component gathered for consistency but unused */
		if (r <= 0)
			return r == EOF ? -1 : OBJ_LP_INVALID;
		return vector3fArray_append(&data->uv, v); /* 0 on success, -1 on error */
		
	} else if (line[0] == 'f' && line[1] == ' '){
		
		int ar = 0;
		char* token, *next;
		
		for (token = strtok_r(line + 2, " ", &next); !ar && token != NULL;
			token = strtok_r(NULL, " ", &next)){
			
			vector3l v = { 0, 0, 0 };
			r = sscanf(token, "%ld/%ld/%ld", &v.x, &v.y, &v.z);
			/* normal index gathered for consistency but unused */
			ar = vector3lArray_append(&data->faceVertices, v);
		}
		return ar; /* 0 on success, -1 on error */
	}
	
	return OBJ_LP_SKIPPED;
}

void objData_free(objData* data){
	
	vector3fArray_free(&data->vertices);
	vector3fArray_free(&data->uv);
	vector3lArray_free(&data->faceVertices);
}
