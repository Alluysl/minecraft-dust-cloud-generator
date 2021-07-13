#include "../../lib/stb_image.h"

#ifndef texture_h_included
#define texture_h_included

typedef struct {
	stbi_uc* pixels;
	int width, height, components;
} texture;

texture* load_texture(char* name);
void get_pixel_RGBA(texture* tex, int x, int y, float* r, float* g, float* b, float* a);
int get_UV_RGBA(texture* tex, float u, float v, int uDirPositive, int vDirPositive, float* r, float* g, float* b, float* a);
void free_texture(texture* tex);

#endif /* #ifndef texture_h_included */
