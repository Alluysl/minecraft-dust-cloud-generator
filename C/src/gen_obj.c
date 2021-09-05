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

	if (argc != 10)
		yeet(
			"Usage:\n"
			"<executable> <input file> <output file> <particle> "
				"<deltaX> <deltaY> <deltaZ> <speed> <count> <force chance>\n"
			"Examples:\n"
			"./gen_obj in.obj out.mcfunction minecraft:flame 0.25 0.25 0.25 0 2 0.5\n"
			"./gen_obj in.obj out.mcfunction "
				"\"minecraft:dust 0.2 0.75 0.95 2.5\" 0 0 0 0 1 0.25\n",
			0);
	
	float x, y, z, speed, bx, by, bz, force;
	int count;
	
	/* argv[3] is the particle */
	if (!get_float_from_string(argv[4], &bx)) yeet("Couldn't parse boxX\n", 1);
	if (!get_float_from_string(argv[5], &by)) yeet("Couldn't parse boxY\n", 1);
	if (!get_float_from_string(argv[6], &bz)) yeet("Couldn't parse boxZ\n", 1);
	if (!get_float_from_string(argv[7], &speed)) yeet("Couldn't parse speed\n", 1);
	if (!get_int_from_string(argv[8], &count)) yeet("Couldn't parse count\n", 1);
	if (!get_float_from_string(argv[9], &force)) yeet("Couldn't parse force chance\n", 1);
	
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
			if (parse_line(buf, &x, &y, &z)
				&& fprintf(fo, "particle %s ~%f ~%f ~%f %f %f %f %f %d %s\n",
					argv[3], x, y, z, bx, by, bz, speed, count,
					force == 1.0 ? "force" : force == 0.0 ? "normal" :
						force * RAND_MAX >= rand() ? "force" : "normal")
					<= 0)
				yeet("Problem while writing to output file\n", 1);
			++lineCount;
		}
	}
	
	fclose(f);
	fclose(fo);
	
	printf("Generated %ld lines in file %s\n", lineCount, argv[2]);
}
