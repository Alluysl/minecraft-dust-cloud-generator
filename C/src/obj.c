#include <stdio.h>
#include <string.h>

#include "obj.h"
#include "util/texture.h"

char* lastError = "No error";

char* objData_get_error(){
	return lastError;
}

objData* objData_new(){
	
	lastError = "Couldn't create OBJ data structure\n";
	
	objData* data = malloc(sizeof(objData));
	if (data == NULL)
		return NULL;
	
	lastError = "Couldn't initialize OBJ data structure\n";
	
	if (vector3fArray_init(&data->vertices, OBJ_FIELD_ARRAY_START_SIZE)){
		free(data);
		return NULL;
	}
	if (vector3fArray_init(&data->uv, OBJ_FIELD_ARRAY_START_SIZE)){
		free(data);
		vector3fArray_free(&data->vertices);
		return NULL;
	}
	if (vector3lArray_init(&data->faceVertices, OBJ_FIELD_ARRAY_START_SIZE)){
		free(data);
		vector3fArray_free(&data->vertices);
		vector3fArray_free(&data->uv);
		return NULL;
	}
	data->vertexColors = NULL;
	
	lastError = "No error";
	
	return data;
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

objLineParsingResult objData_parse_line(objData* data, char* line){
	
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

objData* objData_abort_load(objData* data, char* message){
	lastError = message;
	objData_free(data);
	return NULL;
}

objData* objData_fill_vertex_colors(objData* data, char* texturePath){
	
	texture* tex = NULL;
	if (texturePath == NULL || *texturePath == '\0')
		return data;

	int alreadyWarnedId = 0, alreadyWarnedOOB = 0;
	
	vector4f* vertexColors = malloc(data->vertices.size * sizeof(vector4f));
	if (vertexColors == NULL)
		return objData_abort_load(data, "Couldn't allocate vertex color buffer array\n");
	
	/* uncomment and use -fopenmp option with GCC for parallelization */
	/* (removed due to not being very useful and */
	/* Valgrind complaining about non-leaked "possibly lost" blocks) */
	/* #pragma omp parallel for */
	for (int i = 0; i < data->vertices.size; ++i)
		vertexColors[i] = vector4f_zero();
	
	long* vertexColorContributionCounts = calloc(data->vertices.size, sizeof(long));
	if (vertexColorContributionCounts == NULL){
		free(vertexColors);
		return objData_abort_load(data, "Couldn't allocate vertex color contribution count array\n");
	}
	
	if ((tex = load_texture(texturePath)) == NULL){
		free(vertexColors);
		free(vertexColorContributionCounts);
		return objData_abort_load(data, "Couldn't open texture file\n");
	}
	
	for (size_t i = 0; i < data->faceVertices.size; ++i){
		
		vector3f uv;
		vector3l faceVtx;
		
		/* Get the vertex face */
		faceVtx = vector3lArray_get(&data->faceVertices, i); /* copy, not reference, so can safely edit */
		
		/* Get the vertex and UV indices */
		if (!faceVtx.x || !faceVtx.y){ /* indices start at 1 */
			if (!alreadyWarnedId){
				
				printf("Warning: invalid face vertex (or UV) index, skipping, not warning for future vertices\n");
				alreadyWarnedId = 1;
			}
			continue;
		}
		faceVtx.x += faceVtx.x > 0 ? -1 : data->vertices.size;
		faceVtx.y += faceVtx.y > 0 ? -1 : data->uv.size;
		if (faceVtx.x >= data->vertices.size || faceVtx.y >= data->uv.size
			|| faceVtx.x < 0 || faceVtx.y < 0){
			
			if (!alreadyWarnedOOB){
				if (faceVtx.x >= data->vertices.size || faceVtx.x < 0)
					printf("Warning: invalid vertex index %ld (possibly translated from negative), array size %ld, skipping, not warning for future vertices\n", faceVtx.x + 1, data->vertices.size);
				else
					printf("Warning: invalid UV index %ld (possibly translated from negative), array size %ld, skipping, not warning for future vertices\n", faceVtx.y + 1, data->uv.size);
				alreadyWarnedOOB = 1;
			}
			continue;
		}
		
		/* Get the UV and color at that UV */
		uv = vector3fArray_get(&data->uv, faceVtx.y);
		float r, g, b, a;
		if (!get_UV_RGBA(tex, uv.x, uv.y, 1, 1, &r, &g, &b, &a)){
			
			/* Add color contribution */
			vertexColors[faceVtx.x].w += a;
			vertexColors[faceVtx.x].x += r * a;
			vertexColors[faceVtx.x].y += g * a;
			vertexColors[faceVtx.x].z += b * a;
			++vertexColorContributionCounts[faceVtx.x];
		}
	}
	
	free_texture(tex); /* free image data */
	
	/* uncomment and use -fopenmp option with GCC for parallelization */
	/* (removed due to not being very useful and */
	/* Valgrind complaining about non-leaked "possibly lost" blocks) */
	/* #pragma omp parallel for */
	for (int i = 0; i < data->vertices.size; ++i){
		if (vertexColorContributionCounts[i] && vertexColors[i].w){
			vertexColors[i].x /= vertexColors[i].w;
			vertexColors[i].y /= vertexColors[i].w;
			vertexColors[i].z /= vertexColors[i].w;
			vertexColors[i].w /= vertexColorContributionCounts[i];
		} else
			vertexColors[i] = vector4f_zero();
	}
	
	free(vertexColorContributionCounts);
	data->vertexColors = vertexColors;
	
	return data;
}

#define OBJ_LINE_MAX_SIZE 4048
objData* objData_load_from_file(char* path, char* texturePath){
	
	objData* data = objData_new();
	if (data == NULL)
		return NULL;
	else {
			
		char buf[OBJ_LINE_MAX_SIZE];
		char bin[OBJ_LINE_MAX_SIZE];
		
		long lineCount = 0;
		int keepGoing = 1, plr, alreadyWarned = 0;
		
		FILE* f = fopen(path, "r");
		if (f == NULL)
			return objData_abort_load(data, "Couldn't open input file\n");
		
		while ((keepGoing = !read_short_line_truncated(f, buf, bin, OBJ_LINE_MAX_SIZE))){
			
			if ((plr = objData_parse_line(data, buf)) == OBJ_LP_ERROR)
				return objData_abort_load(data, "Couldn't parse line from input file\n");
				
			if (plr == OBJ_LP_INVALID && !alreadyWarned){
				printf("Warning: invalid line %ld, skipping, not warning for future lines\n", lineCount + 1);
				alreadyWarned = 1;
			}
			
			++lineCount;
		}
		
		if (fclose(f))
			printf("Warning: couldn't close input file\n");
		
		return objData_fill_vertex_colors(data, texturePath);
	}
}

void objData_free(objData* data){
	
	vector3fArray_free(&data->vertices);
	vector3fArray_free(&data->uv);
	vector3lArray_free(&data->faceVertices);
	free(data->vertexColors); /* free(NULL) does nothing by specification */
	free(data);
}
