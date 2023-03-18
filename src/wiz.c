#include "wiz.h"

float lerp(float origin, float target, float t){
	return origin + (target - origin) * t;
}

uint32_t pack_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a){
	return r << 24 | g << 16 | b << 8 | a;
}

void unpack_color(const uint32_t color, 
								uint8_t * r,
								uint8_t * g, 
								uint8_t * b, 
								uint8_t * a)
{
	*r = (color >> 24) & 0xff;
	*g = (color >> 16) & 0xff;
	*b = (color >> 8) & 0xff;
	*a = color & 0xff;
}

void wiz_clear(Canvas * canvas, uint32_t color){
	assert(canvas != NULL);
	for(size_t i = 0; i < canvas->w * canvas->h; i++){
		canvas->pixels[i] = color;
	}
}

void basic_gradient(Canvas * canvas){
	assert(canvas != NULL);
	size_t w = canvas->w;
	size_t h = canvas->h;

	for(size_t y = 0; y < h; y++){
		for(size_t x = 0; x < w; x++){
			uint8_t r = ((float)x / (float)w) * 0xff;
			uint8_t g = ((float)y / (float)h) * 0xff;
			uint32_t color = pack_color(r, g, 0, 0xff);
			canvas->pixels[y * w + x] = color;
		}
	}
}

Canvas * create_basic_canvas(size_t w, size_t h){
	Canvas * c = malloc(sizeof(Canvas));
	assert(c != NULL);

	c->w = w;
	c->h = h;

	c->pixels = malloc(sizeof(uint32_t) * (w * h));
	assert(c->pixels != NULL);

	memset(c->pixels, 0, sizeof(uint32_t) * (w * h));

	return c;
}

void free_basic_canvas(Canvas * canvas){
	assert(canvas != NULL);
	free(canvas->pixels);
	free(canvas);
}
