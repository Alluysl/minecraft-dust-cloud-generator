#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "obj.h"

#define STB_IMAGE_IMPLEMENTATION
#include "lib/stb_image.h"

#define LEN 256

void get_pixel_RGBA(stbi_uc* texture, int width, int components, int x, int y, float* r, float* g, float* b, float* a){
	
	stbi_uc* pixel = texture + components * (width * y + x);
	
	switch (components){
	
	case 4: // RGBA
		
		*r = pixel[0] / 255.0f;
		*g = pixel[1] / 255.0f;
		*b = pixel[2] / 255.0f;
		*a = pixel[3] / 255.0f;
		break;
	
	case 3: // RGB
		
		*r = pixel[0] / 255.0f;
		*g = pixel[1] / 255.0f;
		*b = pixel[2] / 255.0f;
		*a = 1.0f;
		break;
	
	case 2: // GA
		
		*r = *g = *b = pixel[0] / 255.0f;
		*a = pixel[1] / 255.0f;
		break;
	
	default: // G
		
		*r = *g = *b = *pixel / 255.0f;
		*a = 1.0f;
	}
}

int get_UV_RGBA(stbi_uc* texture, int width, int height, int components, float u, float v, float* r, float* g, float* b, float* a){
	
	int x = u * width,
		y = (1.0 - v) * height;
	x -= x == width;
	y -= y == height;
	
	
	// Attempts at fixing stuff, kept here for later reference
	/*double x, y, uu, vv;
	// Wrap repeated
	uu = fmod(u, 1.0);
	vv = fmod(v, 1.0);
	if (uu < 0.0) uu += 1.0;
	if (vv < 0.0) vv += 1.0;
	
	if ((skipIfImageBoundary || skipIfPixelBoundary)
		&& (uu <= skipDelta || vv <= skipDelta))
		return 1;
	
	x = uu * width;
	y = (1.0 - vv) * height;
	
	if (skipIfPixelBoundary){
		
		double bin, xf, yf;
		xf = modf(x, &bin);
		yf = modf(y, &bin);
if (u == 0.671875 && v == 0.1875) printf("u %f v %f uu %lf vv %lf x %lf y %lf xf %lf yf %lf 1.0 - xf %lf 1.0 - yf %lf\n", u, v, uu, vv, x, y, xf, yf, 1.0 - xf, 1.0 - yf);
		if (xf <= skipDelta || yf <= skipDelta
			|| 1.0 - xf  <= skipDelta || 1.0 - yf <= skipDelta)
			return 1;
	}*/
	
	get_pixel_RGBA(texture, width, components, x, y, r, g, b, a);
	
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
	
	FILE* f = fopen(argv[1], "r");
	if (f == NULL) yeet("Couldn't open input file\n", 1);
	
	objData data;
	if (objData_init(&data))
		yeet("Couldn't allocate OBJ data structure\n", 1);
		
	char buf[LEN];
	char bin[LEN];
	
	long lineCount = 0;
	int keepGoing = 1, plr, alreadyWarned = 0;
	while (keepGoing){
		if (keepGoing = !read_short_line_truncated(f, buf, bin, LEN)){
			
			if ((plr = objData_parse_line(buf, &data))){
				if (plr == OBJ_LP_ERROR)
					yeet("Couldn't parse line from input file\n", 1);
				if (plr == OBJ_LP_INVALID && !alreadyWarned){
					printf("Warning: invalid line %ld, skipping, reconstruction could be wrong, not warning for future lines\n", lineCount + 1);
					alreadyWarned = 1;
				}
			}
			++lineCount;
		}
	}
	
	fclose(f); // close input file
	
	float* vertexColors = calloc(data.vertices.size * 4, sizeof(float));
	if (vertexColors == NULL)
		yeet("Couldn't allocate vertex color buffer array\n", 1);
	long* vertexColorContributionCounts = calloc(data.vertices.size, sizeof(long));
	if (vertexColorContributionCounts == NULL)
		yeet("Couldn't allocate vertex color contribution count array\n", 1);
	
	int tw, th, tc;
	stbi_uc* texture = stbi_load(argv[3], &tw, &th, &tc, 0);
	if (texture == NULL) yeet("Couldn't open texture file\n", 0);
	
	alreadyWarned = 0;
	int alreadyWarnedOOB = 0;
	for (size_t i = 0; i < data.faceVertices.size; ++i){
		
		vector3f uv;
		vector3l faceVtx;
		
		// Get the vertex face
		faceVtx = vector3lArray_get(&data.faceVertices, i); // copy, not reference, so can safely edit
		
		// Get the vertex and UV indices
		if (!faceVtx.x || !faceVtx.y){ // indices start at 1
			if (!alreadyWarned){
				
				printf("Warning: invalid face vertex (or UV) index, skipping, not warning for future vertices\n");
				alreadyWarned = 1;
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
		if (!get_UV_RGBA(texture, tw, th, tc, uv.x, uv.y, &r, &g, &b, &a)){
			
			// Add color contribution
			vertexColors[faceVtx.x * 4] += a;
			vertexColors[faceVtx.x * 4 + 1] += r * a;
			vertexColors[faceVtx.x * 4 + 2] += g * a;
			vertexColors[faceVtx.x * 4 + 3] += b * a;
			++vertexColorContributionCounts[faceVtx.x];
		}
	}
	
	stbi_image_free(texture); // free image data
	
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
			force == 1.0 ? "force" : force == 0.0 ? "normal" : force * RAND_MAX >= rand() ? "force" : "normal") <= 0)
			yeet("Problem while writing to output file\n", 1);
		
		++lineCount;
	}
	
	fclose(fo); // close output file
	free(vertexColors);
	free(vertexColorContributionCounts);
	
	printf("Generated %ld lines in file %s\n", data.vertices.size - skipped, argv[2]);
	objData_free(&data); // free OBJ data
}
