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

int read_short_line_truncated(FILE* f, char* buf, char* bin, int size){
	char* p = fgets(buf, size, f);
	if (p == NULL)
		return 1;
	buf[size-1] = '\0';
	p = strrchr(buf, '\n');
	while (p == NULL){
		p = fgets(bin, size, f);
		if (p == NULL)
			return 1;
		p = strrchr(buf, '\n');
	}
	*p = '\0';
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

#define OBJ_LINE_MAX_SIZE 4048
int objData_load_from_file(objData* data, char* path, char** err){
		
	char buf[OBJ_LINE_MAX_SIZE];
	char bin[OBJ_LINE_MAX_SIZE];
	
	long lineCount = 0;
	int keepGoing = 1, plr, alreadyWarned = 0;
	
	FILE* f = fopen(path, "r");
	if (f == NULL){
		if (err != NULL)
			*err = "Couldn't open input file\n"; /* string literals are global and unchanging */
		return -1;
	}
	
	while (keepGoing){
		if (keepGoing = !read_short_line_truncated(f, buf, bin, OBJ_LINE_MAX_SIZE)){
			
			if ((plr = objData_parse_line(buf, data))){
				if (plr == OBJ_LP_ERROR){
					if (err != NULL)
						*err = "Couldn't parse line from input file\n";
					return -1;
				}
				if (plr == OBJ_LP_INVALID && !alreadyWarned){
					printf("Warning: invalid line %ld, skipping, not warning for future lines\n", lineCount + 1);
					alreadyWarned = 1;
				}
			}
			++lineCount;
		}
	}
	
	return fclose(f);
}

void objData_free(objData* data){
	
	vector3fArray_free(&data->vertices);
	vector3fArray_free(&data->uv);
	vector3lArray_free(&data->faceVertices);
}
