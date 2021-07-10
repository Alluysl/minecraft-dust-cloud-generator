#include <stdio.h>

#define XRAD 0.375
#define XSTRIDE 0.125

#define YMIN -0.5
#define YMAX 1.4
#define YSTRIDE 0.125

int main(){
	
	for (float y = YMIN; y <= YMAX; y += YSTRIDE)
		for (float x = -XRAD; x <= XRAD; x += XSTRIDE)
			for (float z = -XRAD; z <= XRAD; z += XSTRIDE)
				printf("particle minecraft:dust 0 0.8 1 0.5 ~%f ~%f ~%f 0 0 0 0 8 force\n", x, y, z);
	printf("Printed %ld lines or so.\n", (long)(4*(YMAX - YMIN)/YSTRIDE*XRAD/XSTRIDE*XRAD/XSTRIDE));
	
	return 0;
}
