#include <stdio.h>
#include <string.h>
#include <math.h>
#include <errno.h>

#include "obj.h"
#include "texture.h"

#if defined(_WIN32) || defined(_WIN64)
/* Same signature, thank you StackOverflow */
#define strtok_r strtok_s
#endif

char* lastError = "No error";
int isSystemError = 0;

char* objData_get_error(int* isSystem){
	if (isSystem != NULL)
		*isSystem = isSystemError;
	return lastError;
}

objData* objData_new(){
	
	lastError = "Couldn't create OBJ data structure\n";
	isSystemError = 1;
	
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
	if (faceArray_init(&data->faces, OBJ_FIELD_ARRAY_START_SIZE)){
		free(data);
		vector3fArray_free(&data->vertices);
		vector3fArray_free(&data->uv);
		return NULL;
	}
	data->vertexColors = NULL;
	
	lastError = "No error";
	isSystemError = 0;
	
	return data;
}

objLineReadingResult read_line(FILE* f, char** line,
	char* buffer, size_t bufferSize){
	/* buffer avoids alloc if line is short enough */
	
	fpos_t fpos;
	char* res, *r;
	int c;
	size_t size = 0;
	
	/* Get line length */
	
	if (fgetpos(f, &fpos))
		return -1;	
	for (c = fgetc(f); c != '\n' && c != EOF; ++size, c = fgetc(f));
	if (fsetpos(f, &fpos))
		return -1;
	
	if (!size){
		if (c == '\n'){
			fgetc(f); /* consume line */
			return OBJ_LR_EMPTY;
		} else
			return OBJ_LR_EOF;
	} else
		size += 1 + (c == '\n'); /* add space for (\n)\0 */
	
	/* Allocate needed memory and pull line */
	
	if (size > bufferSize){
		res = malloc(size); /* sizeof(char) == 1 by definition */
		if (res == NULL)
			return -1;
	} else
		res = buffer;
	
	if (fgets(res, size, f) == NULL){
		/* has to be caused by an error because we already covered */
		/* the possibility of the line being empty & last (EOF only) */
		if (res != buffer)
			free(res);
		return -1;
	}
	
	/* Remove new line and carriage return characters if present and return */
	
	if ((r = strrchr(res, '\n')) != NULL)
		*r = '\0';
	if ((r = strrchr(res, '\r')) != NULL)
		*r = '\0';
	
	*line = res;
	return 0;
}

objLineParsingResult objData_parse_line(objData* data, char* line){
	
	int r;
	if (line[0] == 'v' && line[1] == ' '){
		
		vector3f v;
		r = sscanf(line + 2, "%f %f %f", &v.x, &v.y, &v.z);
		if (r != 3)
			return r == EOF ? -1 : OBJ_LP_INVALID;  /* OBJ_LP_INVALID on missing data, -1 on error */
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
		vector3lArray face;
		
		if (vector3lArray_init(&face, 4))
			return -1;
		
		for (token = strtok_r(line + 2, " ", &next); !ar && token != NULL;
			token = strtok_r(NULL, " ", &next)){
			
			vector3l v = { 0, 0, 0 };
			r = sscanf(token, "%ld/%ld/%ld", &v.x, &v.y, &v.z);
			/* normal index gathered for consistency but unused */
			if (r <= 0){
				vector3lArray_free(&face);
				return r == EOF ? -1 : OBJ_LP_INVALID;
			}
			ar = vector3lArray_append(&face, v);
		}
		if (!ar){
			ar = faceArray_append(&data->faces, face);
		}
		return ar; /* 0 on success, -1 on error */
	}
	
	return OBJ_LP_SKIPPED;
}

int objData_get_face_vertex_ids(objData* data, vector3lArray face,
	size_t faceVertexId, size_t* vertexId, size_t* uvId){
	
	/* Get indices and map from ]-inf, +inf[ to [1, +inf[ (relative/absolute to absolute) */
	
	vector3l faceVertex = vector3lArray_get(&face, faceVertexId);
	*vertexId = faceVertex.x + (faceVertex.x < 0) * (data->vertices.size + 1);
	*uvId     = faceVertex.y + (faceVertex.y < 0) * (data->uv.size + 1);
	
	if (!*vertexId || !*uvId){ /* indices start at 1 */
		
		fprintf(stderr, "Invalid vertex/UV index %ld/%ld, skipping, not warning for future vertices\n",
			faceVertex.x, faceVertex.y);
		return 1;
	}
	
	/* Check if resulting index is in bounds of gathered indices */
	
	if (*vertexId > data->vertices.size || *uvId > data->uv.size){
		
		if (*vertexId > data->vertices.size || *vertexId <= 0)
			fprintf(stderr, "Invalid vertex index %lu (from %ld), "
					"array size %ld, skipping, not warning for future vertices\n",
				*vertexId, faceVertex.x, data->vertices.size);
		else
			fprintf(stderr, "Invalid UV index %lu (from %ld), "
					"array size %ld, skipping, not warning for future vertices\n",
				*uvId, faceVertex.y, data->uv.size);
		return 1;
	}
	
	return 0;
}

objData* objData_abort_load(objData* data, char* message, int isSystem){
	lastError = message;
	isSystemError = isSystem;
	objData_free(data);
	return NULL;
}

static inline int isDoublePositive(double d){
	return !((unsigned long long)d & ((unsigned long long)1 << 63)); /* IEEE 754 fun */
}

objData* objData_fill_vertex_colors(objData* data, char* texturePath, double pixelFloatPrecision){
	
	texture* tex = NULL;
	if (texturePath == NULL || *texturePath == '\0')
		return data;
	
	vector4f* vertexColors = malloc(data->vertices.size * sizeof(vector4f));
	if (vertexColors == NULL)
		return objData_abort_load(data, "Couldn't allocate vertex color buffer array\n", 1);
	
	for (size_t i = 0; i < data->vertices.size; ++i)
		vertexColors[i] = vector4f_zero();
	
	long* vertexColorContributionCounts = calloc(data->vertices.size, sizeof(long));
	if (vertexColorContributionCounts == NULL){
		free(vertexColors);
		return objData_abort_load(data, "Couldn't allocate vertex color contribution count array\n", 1);
	}
	
	if ((tex = load_texture(texturePath)) == NULL){
		free(vertexColors);
		free(vertexColorContributionCounts);
		return objData_abort_load(data, "Couldn't open texture file\n", 1);
	}
	
	for (size_t faceId = 0; faceId < data->faces.size; ++faceId){
		
		size_t* vertexIds = NULL; /* set to NULL so can be freed, simplifies logic */
		vector3f* uvs = NULL;     /* same thing */
		char* turns = NULL;       /* same thing */
		vector3lArray face = faceArray_get(&data->faces, faceId);
		
		if (face.size < 3) /* somehow? */
			continue;
		
		if ((vertexIds = malloc(face.size * sizeof(size_t))) == NULL
			|| (turns = malloc(face.size)) == NULL /* sizeof(char) == 1 by definition */
			|| (uvs = malloc(face.size * sizeof(vector3f))) == NULL){
			
			free(vertexIds);
			free(turns);
			free(vertexColors);
			free(vertexColorContributionCounts);
			return objData_abort_load(data, "Couldn't allocate memory "
				"while processing faces for vertex colors\n", 1);
		}
		
		double totalAngle = 0.0;
		
		/* Iterate over face vertices to get corresponding model vertices and UVs */
		
		for (size_t i = 0; i < face.size + 2; ++i){
			
			size_t uvId;
			
			if (i < face.size){
			
				if (objData_get_face_vertex_ids(data, face, i, vertexIds + i, &uvId)){
					
					free(vertexIds); free(uvs); free(turns);
					free(vertexColors); free(vertexColorContributionCounts);
					return objData_abort_load(data, "Error getting indices from face\n", 0);
				}
				
				uvs[i] = vector3fArray_get(&data->uv, uvId - 1);
			}
			if (i >= 2){ /* can use this vertex and previous two to get angle */
				
				/* ingoing/outgoing edge vectors */
				vector3f a = vector3f_difference(uvs[i - 2], uvs[(i - 1) % face.size]), /* in */
					b = vector3f_difference(uvs[(i - 1) % face.size], uvs[i % face.size]); /* out*/
				
				double angle = atan2(a.x * b.y - a.y * b.x, a.x * b.x + a.y * b.y);
				turns[(i - 1) % face.size] = isDoublePositive(angle);
				totalAngle += angle;
			}
		}
		
		for (size_t i = 0; i < face.size; ++i){
			
			vector3f bisector;
			size_t vertexId = vertexIds[i];
			
			if (!vertexId--)
				continue;
			
			/* Get bisector of angle at vertex from neighboring vertices */
			
			bisector = vector3f_add(
					vector3f_difference(uvs[i], uvs[i - 1 + !i * face.size]),
					vector3f_difference(uvs[i], uvs[(i + 1) % face.size])
				);
			
			if (turns[i] ^ isDoublePositive(totalAngle)) /* angle at vertex is concave */
				bisector = vector3f_subtract(vector3f_zero(), bisector);
		
			/* Get the color at the UV of the vertex on the current face */
			float r, g, b, a;
			if (!get_UV_RGBA(tex, uvs[i].x, uvs[i].y,
				bisector.x >= 0, bisector.y >= 0, pixelFloatPrecision,
				&r, &g, &b, &a)){
				
				/* Add color contribution */
				vertexColors[vertexId].w += a;
				vertexColors[vertexId].x += r * a;
				vertexColors[vertexId].y += g * a;
				vertexColors[vertexId].z += b * a;
				++vertexColorContributionCounts[vertexId];
			}
		}
		
		free(vertexIds);
		free(uvs);
		free(turns);
	}
	
	free_texture(tex); /* free image data */
	
	for (size_t i = 0; i < data->vertices.size; ++i){
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

#define OBJ_LINE_BUF_SIZE 256 /* malloc if higher */
objData* objData_load_from_file(char* path, char* texturePath, double pixelFloatPrecision){
	
	objData* data = objData_new();
	if (data == NULL)
		return NULL;
	else {
		
		char* readLine;
		char buf[OBJ_LINE_BUF_SIZE];
		
		long lineCount = 0;
		int keepGoing = 1, plr, alreadyWarned = 0;
		
		/* opening file in binary mode and not text mode so that  */
		/* jumping around actually works on Windows (CRLF issues) */
		FILE* f = fopen(path, "rb");
		if (f == NULL)
			return objData_abort_load(data, "Couldn't open input file\n", 1);
		
		while (keepGoing){
			
			switch (read_line(f, &readLine, buf, OBJ_LINE_BUF_SIZE)){
			
			case -1: /* error */
				return objData_abort_load(data, "Couldn't read line from input file\n", 1);
			
			case 0: /* line successfully read */
				
				if ((plr = objData_parse_line(data, readLine)) == OBJ_LP_ERROR){
					if (readLine != buf)
						free(readLine);
					return objData_abort_load(data, "Couldn't parse line from input file\n", 1);
				}
					
				if (plr == OBJ_LP_INVALID && !alreadyWarned){
					printf("Warning: invalid line %ld, skipping, not warning for future lines\n",
						lineCount + 1);
					alreadyWarned = 1;
				}
				
				++lineCount;
				if (readLine != buf)
					free(readLine);
				break;
			
			case OBJ_LR_EOF: /* nothing left to read */
				keepGoing = 0;
				break;
			
			case OBJ_LR_EMPTY: /* do nothing in empty case */
				break;
			}
		}
		
		if (fclose(f)) /* not enough of a sole reason to drop everything but still warning */
			printf("Warning: couldn't close input file: %s\n", strerror(errno));
		
		return objData_fill_vertex_colors(data, texturePath, pixelFloatPrecision);
	}
}

void objData_free(objData* data){
	
	vector3fArray_free(&data->vertices);
	vector3fArray_free(&data->uv);
	faceArray_free(&data->faces);
	free(data->vertexColors); /* free(NULL) does nothing by specification */
	free(data);
}
