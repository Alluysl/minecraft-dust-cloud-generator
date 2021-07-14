
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

int get_UV_RGBA(texture* tex, float u, float v,
	int uDirPositive, int vDirPositive, double pixelFloatPrecision,
	float* r, float* g, float* b, float* a){
	
	double uu, vv, x, y, xf, yf, bin;
	int xi, yi;
	
	/* Wrap repeated */
	uu = fmod(u, 1.0);
	vv = fmod(v, 1.0);
	if (uu < 0.0) uu += 1.0;
	if (vv < 0.0) vv += 1.0;
	
	/* Get floating-point pixel coordinate */
	x = uu * tex->width;
	y = (1.0 - vv) * tex->height;
	
	/* Get its decimal part */
	xf = modf(x, &bin);
	yf = modf(y, &bin);
	
	/* If falls on pixel border, make sure the right pixel is sampled*/
	if (!uDirPositive && xf <= pixelFloatPrecision)
		x -= 0.5;
	else if (uDirPositive && pixelFloatPrecision + xf >= 1.0)
		x += 0.5;
	if (vDirPositive && yf <= pixelFloatPrecision)
		y -= 0.5;
	else if (!vDirPositive && pixelFloatPrecision + yf >= 1.0)
		y += 0.5;
	
	/* Cast */
	xi = x;
	yi = y;
	
	/* Handle borders of screen */
	if (xi < 0)
		xi += tex->width;
	else if (xi >= tex->width)
		xi -= tex->width;
	if (yi < 0)
		yi += tex->height;
	else if (yi >= tex->height)
		yi -= tex->height;
	
	get_pixel_RGBA(tex, xi, yi, r, g, b, a);
	
	return 0;
}

void free_texture(texture* tex){
	stbi_image_free(tex->pixels);
	free(tex);
}
