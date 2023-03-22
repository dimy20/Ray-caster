#pragma once

#include <SDL2/SDL.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "engine.h"

#define CELL_SIZE 64 // cube of dimensions 64 x 64 x 64
typedef struct{
	int * values;
	size_t w;
	size_t h;
	SDL_Rect viewport;
	size_t cell_size;
}Map;

void map_init(Map * map, int * values, size_t w, size_t h, SDL_Rect viewport);
void map_quit(Map * map);
void map_draw(const Map * map, SDL_Renderer * renderer);
