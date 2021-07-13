SRC = src
UTIL = src/util

gen_obj_color: $(SRC)/gen_obj_color.c $(SRC)/obj.c $(SRC)/obj.h $(UTIL)/texture.c $(UTIL)/texture.h $(UTIL)/vector.c $(UTIL)/vector.h
	gcc -o $@ -O2 $(SRC)/gen_obj_color.c $(SRC)/obj.c $(UTIL)/texture.c $(UTIL)/vector.c -lm -I lib

gen_obj: $(SRC)/gen_obj.c
	gcc -o $@ -O2 $(SRC)/gen_obj.c

gen_obj_example: gen_obj
	./gen_obj shape.obj red_shadow.mcfunction 0.75 0.25 0.25 0.5 0 0 0 0 1 0.25
	./gen_obj shape.obj cyan_shadow.mcfunction 0.0 0.8 1.0 0.5 0 0 0 0 1 0.25

gen_grid: $(SRC)/gen_grid.c
	gcc -o $@ -O2 $(SRC)/gen_grid.c

clean:
	rm gen_obj_color gen_obj gen_grid
