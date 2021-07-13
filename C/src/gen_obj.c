#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LEN 256

int read_short_line_truncated(FILE* f, char* buf, char* bin, int size){
	char* p = fgets(buf, size, f);
	if (p == NULL)
		return 0;
	buf[size-1] = '\0';
	p = strrchr(buf, '\n');
	while (p == NULL){
		p = fgets(bin, size, f);
		if (p == NULL)
			return 0;
		p = strrchr(buf, '\n');
	}
	*p = '\0';
	return 1;
}

int parse_line(char* line, float* x, float* y, float* z){
	if (*line != 'v')
		return 0;
	int r = sscanf(line, "v %f %f %f", x, y, z);
	return r == 3;
}

int get_int_from_string(char* str, int* f){
	return sscanf(str, "%d", f) == 1;
}

int get_float_from_string(char* str, float* f){
	return sscanf(str, "%f", f) == 1;
}

void yeet(char* message, int usePerror){
	if (message != NULL)
		fprintf(stderr, "%s", message);
	if (usePerror)
		perror("");
	exit(EXIT_FAILURE);
}

int main(int argc, char* argv[]){

	if (argc != 13)
		yeet("Usage: <executable> <input file> <output file> <r> <g> <b> <size> <boxX> <boxY> <boxZ> <speed> <count> <force chance>\n", 0);
	
	float x, y, z, r, g, b, size, speed, bx, by, bz, force;
	int count;
	
	if (!get_float_from_string(argv[3], &r)) yeet("Couldn't parse red\n", 1);
	if (!get_float_from_string(argv[4], &g)) yeet("Couldn't parse green\n", 1);
	if (!get_float_from_string(argv[5], &b)) yeet("Couldn't parse blue\n", 1);
	if (!get_float_from_string(argv[6], &size)) yeet("Couldn't parse size\n", 1);
	if (!get_float_from_string(argv[7], &bx)) yeet("Couldn't parse boxX\n", 1);
	if (!get_float_from_string(argv[8], &by)) yeet("Couldn't parse boxY\n", 1);
	if (!get_float_from_string(argv[9], &bz)) yeet("Couldn't parse boxZ\n", 1);
	if (!get_float_from_string(argv[10], &speed)) yeet("Couldn't parse speed\n", 1);
	if (!get_int_from_string(argv[11], &count)) yeet("Couldn't parse count\n", 1);
	if (!get_float_from_string(argv[12], &force)) yeet("Couldn't parse force chance\n", 1);
	
	FILE* f = fopen(argv[1], "r");
	if (f == NULL) yeet("Couldn't open input file\n", 1);
	FILE* fo = fopen(argv[2], "w");
	if (fo == NULL) yeet("Couldn't open output file\n", 1);
	
	char buf[LEN];
	char bin[LEN];
	
	if (fputs("# Generated using https://github.com/Alluysl/minecraft-dust-cloud-generator\n", fo) == EOF)
		yeet("Problem while writing to output file\n", 1);
	
	long lineCount = 0;
	int keepGoing = 1;
	while (keepGoing){
		keepGoing = read_short_line_truncated(f, buf, bin, LEN);
		if (keepGoing){
			if (parse_line(buf, &x, &y, &z) && fprintf(fo, "particle minecraft:dust %f %f %f %f ~%f ~%f ~%f %f %f %f %f %d %s\n", r, g, b, size, x, y, z, bx, by, bz, speed, count, force == 1.0 ? "force" : force == 0.0 ? "normal" : force * RAND_MAX >= rand() ? "force" : "normal") <= 0) yeet("Problem while writing to output file\n", 1);
			++lineCount;
		}
	}
	
	fclose(f);
	fclose(fo);
	
	printf("Generated %ld lines in file %s\n", lineCount, argv[2]);
}
