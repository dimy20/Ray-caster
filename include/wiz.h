#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

typedef struct{
	uint32_t * pixels;
	size_t w;
	size_t h;
}Canvas;

uint32_t pack_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
void unpack_color(const uint32_t color, 
								uint8_t * r,
								uint8_t * g, 
								uint8_t * b, 
								uint8_t * a);

void clear(Canvas * canvas, uint32_t color);
void basic_gradient(Canvas * canvas);

Canvas * create_basic_canvas(size_t w, size_t h);
void free_basic_canvas(Canvas * canvas);
