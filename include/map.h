#pragma once

#include <SDL2/SDL.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "engine.h"
#include "rc_math.h"

#define CELL_SIZE 64 // cube of dimensions 64 x 64 x 64

typedef struct{
	int * values;
	size_t w;
	size_t h;
	size_t cell_size;
	const SDL_Rect * viewport;
	const uint32_t * colors;
}Map;

void map_init(Map * map, int * values, size_t w, size_t h, const SDL_Rect * viewport);
void map_quit(Map * map);
void map_draw(const Map * map, SDL_Renderer * renderer, size_t window_w, size_t window_h);
void world_2_screen(const Map * map, const vec2f * world_pos, vec2i * screen);
