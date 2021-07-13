#include "lib/stb_image.h"

#ifndef texture_h_included
#define texture_h_included

typedef stbi_uc texture;

texture* load_texture(char* name, int* width, int* height, int* components);
void get_pixel_RGBA(texture* tex, int width, int components, int x, int y, float* r, float* g, float* b, float* a);
int get_UV_RGBA(texture* tex, int width, int height, int components, float u, float v, int udir, int vdir, float* r, float* g, float* b, float* a);
void free_texture(texture* tex);

#endif /* #ifndef texture_h_included */
