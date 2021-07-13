#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "obj.h"
#include "texture.h"

void yeet(char* message, int usePerror){
	if (message != NULL)
		fprintf(stderr, "%s", message);
	if (usePerror)
		perror("");
	exit(EXIT_FAILURE);
}

int isYes(char c){ return c == 'y' || c == 'Y' || c == '1'; }
int isNo(char c){ return c == 'n' || c == 'N' || c == '0'; }

int main(int argc, char* argv[]){
	
	int r;
	
	float size, speed, bx, by, bz, force;
	int count;
	
	if (argc != 11)
		yeet("Usage: <executable> <input file> <output file> <RGB-driving image file> <size> <boxX> <boxY> <boxZ> <speed> <count> <force chance>\n", 0);
	
	if ((r = sscanf(argv[4], "%f", &size)) != 1) yeet("Couldn't parse size\n", r == -1);
	if ((r = sscanf(argv[5], "%f", &bx)) != 1) yeet("Couldn't parse boxX\n", r == -1);
	if ((r = sscanf(argv[6], "%f", &by)) != 1) yeet("Couldn't parse boxY\n", r == -1);
	if ((r = sscanf(argv[7], "%f", &bz)) != 1) yeet("Couldn't parse boxZ\n", r == -1);
	if ((r = sscanf(argv[8], "%f", &speed)) != 1) yeet("Couldn't parse speed\n", r == -1);
	if ((r = sscanf(argv[9], "%d", &count)) != 1) yeet("Couldn't parse count\n", r == -1);
	if ((r = sscanf(argv[10], "%f", &force)) != 1) yeet("Couldn't parse force chance\n", r == -1);
	
	objData data;
	if (objData_init(&data))
		yeet("Couldn't allocate OBJ data structure\n", 1);
	
	char* err;
	if (objData_load_from_file(&data, argv[1], &err)){
		yeet(err, 1);
	}
	
	float* vertexColors = calloc(data.vertices.size * 4, sizeof(float));
	if (vertexColors == NULL)
		yeet("Couldn't allocate vertex color buffer array\n", 1);
	long* vertexColorContributionCounts = calloc(data.vertices.size, sizeof(long));
	if (vertexColorContributionCounts == NULL)
		yeet("Couldn't allocate vertex color contribution count array\n", 1);
	
	int tw, th, tc;
	texture* tex = load_texture(argv[3], &tw, &th, &tc);
	if (tex == NULL) yeet("Couldn't open texture file\n", 0);
	
	int alreadyWarnedId = 0, alreadyWarnedOOB = 0;
	for (size_t i = 0; i < data.faceVertices.size; ++i){
		
		vector3f uv;
		vector3l faceVtx;
		
		// Get the vertex face
		faceVtx = vector3lArray_get(&data.faceVertices, i); // copy, not reference, so can safely edit
		
		// Get the vertex and UV indices
		if (!faceVtx.x || !faceVtx.y){ // indices start at 1
			if (!alreadyWarnedId){
				
				printf("Warning: invalid face vertex (or UV) index, skipping, not warning for future vertices\n");
				alreadyWarnedId = 1;
			}
			continue;
		}
		faceVtx.x += faceVtx.x > 0 ? -1 : data.vertices.size;
		faceVtx.y += faceVtx.y > 0 ? -1 : data.uv.size;
		if (faceVtx.x >= data.vertices.size || faceVtx.y >= data.uv.size
			|| faceVtx.x < 0 || faceVtx.y < 0){
			
			if (!alreadyWarnedOOB){
				if (faceVtx.x >= data.vertices.size || faceVtx.x < 0)
					printf("Warning: invalid vertex index %ld (possibly translated from negative), array size %ld, skipping, not warning for future vertices\n", faceVtx.x + 1, data.vertices.size);
				else
					printf("Warning: invalid UV index %ld (possibly translated from negative), array size %ld, skipping, not warning for future vertices\n", faceVtx.y + 1, data.uv.size);
				alreadyWarnedOOB = 1;
			}
			continue;
		}
		
		// Get the UV and color at that UV
		uv = vector3fArray_get(&data.uv, faceVtx.y);
		float r, g, b, a;
		if (!get_UV_RGBA(tex, tw, th, tc, uv.x, uv.y, 1, 1, &r, &g, &b, &a)){
			
			// Add color contribution
			vertexColors[faceVtx.x * 4] += a;
			vertexColors[faceVtx.x * 4 + 1] += r * a;
			vertexColors[faceVtx.x * 4 + 2] += g * a;
			vertexColors[faceVtx.x * 4 + 3] += b * a;
			++vertexColorContributionCounts[faceVtx.x];
		}
	}
	
	free_texture(tex); // free image data
	
	FILE* fo = fopen(argv[2], "w");
	if (fo == NULL) yeet("Couldn't open output file\n", 1);
	
	if (fputs("# Generated using https://github.com/Alluysl/minecraft-dust-cloud-generator\n", fo) == EOF)
		yeet("Problem while writing to output file\n", 1);
	
	size_t skipped = 0;
	
	for (size_t i = 0; i < data.vertices.size; ++i){
		
		float r, g, b, a;
		int div;
		vector3f v = vector3fArray_get(&data.vertices, i);
		div = vertexColorContributionCounts[i];
		if (!div || !(a = vertexColors[i * 4])){
			++skipped;
			continue; // no contribution (vertex was cancelled or is fully transparent)
		}
		
		r = vertexColors[i * 4 + 1] / a;
		g = vertexColors[i * 4 + 2] / a;
		b = vertexColors[i * 4 + 3] / a;
		a /= div;
		
		if (fprintf(fo, "particle minecraft:dust %f %f %f %f ~%f ~%f ~%f %f %f %f %f %d %s\n",
			r, g, b, a * size,
			v.x, v.y, v.z, bx, by, bz,
			speed, count,
			a * force == 1.0f ? "force" : a * force == 0.0f ? "normal" : a * force * RAND_MAX >= rand() ? "force" : "normal") <= 0)
			yeet("Problem while writing to output file\n", 1);
	}
	
	fclose(fo); // close output file
	free(vertexColors);
	free(vertexColorContributionCounts);
	
	printf("Generated %ld lines in file %s\n", data.vertices.size - skipped, argv[2]);
	objData_free(&data); // free OBJ data
}
