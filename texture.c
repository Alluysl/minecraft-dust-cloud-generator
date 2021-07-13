
#define STB_IMAGE_IMPLEMENTATION

#include "texture.h"

texture* load_texture(char* name){
	
	texture* newTex = malloc(sizeof(texture));
	if (newTex != NULL && (newTex->pixels = stbi_load(name, &newTex->width, &newTex->height, &newTex->components, 0)) == NULL){
		free(newTex);
		newTex = NULL;
	}
	return newTex;
}

void get_pixel_RGBA(texture* tex, int x, int y, float* r, float* g, float* b, float* a){
	
	stbi_uc* pixel = tex->pixels + tex->components * (tex->width * y + x);
	
	switch (tex->components){
	
	case 4: /* RGBA */
		
		*r = pixel[0] / 255.0f;
		*g = pixel[1] / 255.0f;
		*b = pixel[2] / 255.0f;
		*a = pixel[3] / 255.0f;
		break;
	
	case 3: /* RGB */
		
		*r = pixel[0] / 255.0f;
		*g = pixel[1] / 255.0f;
		*b = pixel[2] / 255.0f;
		*a = 1.0f;
		break;
	
	case 2: /* GA */
		
		*r = *g = *b = pixel[0] / 255.0f;
		*a = pixel[1] / 255.0f;
		break;
	
	default: /* G */
		
		*r = *g = *b = *pixel / 255.0f;
		*a = 1.0f;
	}
}

int get_UV_RGBA(texture* tex, float u, float v, int udir, int vdir, float* r, float* g, float* b, float* a){
	
	int x = u * tex->width,
		y = (1.0 - v) * tex->height;
	x -= x == tex->width;
	y -= y == tex->height;
	
	
	/* Attempts at fixing stuff, kept here for later reference */
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
	
	get_pixel_RGBA(tex, x, y, r, g, b, a);
	
	return 0;
}

void free_texture(texture* tex){
	stbi_image_free(tex->pixels);
	free(tex);
}
