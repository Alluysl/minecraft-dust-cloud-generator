gen_obj_color: gen_obj_color.c texture.c texture.h obj.c obj.h vector3.c vector3.h
	gcc -o $@ -O2 gen_obj_color.c texture.c obj.c vector3.c -lm -I lib

gen_obj: gen_obj.c
	gcc -o $@ -O2 gen_obj.c

gen_obj_example: gen_obj
	./gen_obj shape.obj red_shadow.mcfunction 0.75 0.25 0.25 0.5 0 0 0 0 1 0.25
	./gen_obj shape.obj cyan_shadow.mcfunction 0.0 0.8 1.0 0.5 0 0 0 0 1 0.25

gen_grid: gen_grid.c
	gcc -o $@ -O2 gen_grid.c

clean:
	rm gen_obj_color gen_obj gen_grid
