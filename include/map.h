#pragma once

#include <SDL2/SDL.h>

typedef struct{
	int * values;
	size_t w;
	size_t h;
	SDL_Rect viewport;
}Map;

void map_init(Map * map, int * values, size_t w, size_t h, SDL_Rect viewport);
void map_draw(const Map * map, SDL_Renderer * renderer);
