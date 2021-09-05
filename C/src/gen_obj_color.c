#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util/obj.h"

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
	double pixelFloatPrecision = 0.0;
	
	if (argc != 11 && argc != 12)
		yeet("Usage: <executable> <input file> <output file> <RGB-driving image file> <size> <boxX> <boxY> <boxZ> <speed> <count> <force chance> [<pixel precision>]\n", 0);
	
	if ((r = sscanf(argv[4], "%f", &size)) != 1) yeet("Couldn't parse size\n", r == -1);
	if ((r = sscanf(argv[5], "%f", &bx)) != 1) yeet("Couldn't parse boxX\n", r == -1);
	if ((r = sscanf(argv[6], "%f", &by)) != 1) yeet("Couldn't parse boxY\n", r == -1);
	if ((r = sscanf(argv[7], "%f", &bz)) != 1) yeet("Couldn't parse boxZ\n", r == -1);
	if ((r = sscanf(argv[8], "%f", &speed)) != 1) yeet("Couldn't parse speed\n", r == -1);
	if ((r = sscanf(argv[9], "%d", &count)) != 1) yeet("Couldn't parse count\n", r == -1);
	if ((r = sscanf(argv[10], "%f", &force)) != 1) yeet("Couldn't parse force chance\n", r == -1);
	if (argc > 11 && (r = sscanf(argv[11], "%lf", &pixelFloatPrecision)) != 1) yeet("Couldn't parse pixel precision\n", r == -1);
	
	objData* data = objData_load_from_file(argv[1], argv[3], pixelFloatPrecision);
	if (data == NULL){
		char* err = objData_get_error(&r);
		yeet(err, r);
	}
	
	FILE* fo = fopen(argv[2], "w");
	if (fo == NULL) yeet("Couldn't open output file\n", 1);
	
	if (fputs("# Generated using https://github.com/Alluysl/minecraft-dust-cloud-generator\n", fo) == EOF)
		yeet("Problem while writing to output file\n", 1);
	
	size_t skipped = 0;
	char isTextureNull = data->vertexColors == NULL;
	
	if (isTextureNull)
		printf("Warning: null texture file, making particles black.\n");
	
	for (size_t i = 0; i < data->vertices.size; ++i){
		
		vector3f v = vector3fArray_get(&data->vertices, i);
		vector4f c = { 0.0f, 0.0f, 0.0f, 1.0f };
		if (!isTextureNull)
			c = data->vertexColors[i];
		
		if (!c.w){
			++skipped;
			continue; // no contribution (vertex was cancelled or is fully transparent)
		}
		
		if (fprintf(fo, "particle minecraft:dust %f %f %f %f ~%f ~%f ~%f %f %f %f %f %d %s\n",
			c.x, c.y, c.z, c.w * size, /* color and size */
			v.x, v.y, v.z, bx, by, bz, /* position and bounding box */
			speed, count,
			c.w * force == 1.0f ? "force" : c.w * force == 0.0f ? "normal" : c.w * force * RAND_MAX >= rand() ? "force" : "normal") <= 0)
			yeet("Problem while writing to output file\n", 1);
	}
	
	fclose(fo); // close output file
	
	printf("Generated %ld lines in file %s\n", data->vertices.size - skipped, argv[2]);
	objData_free(data); // free OBJ data
}
