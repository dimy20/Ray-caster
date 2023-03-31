#pragma once

#include <SDL2/SDL.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "engine.h"
#include "rc_math.h"

#define CELL_SIZE 64 // cube of dimensions 64 x 64 x 64
#define WALL_BIT 0x1
#define FLOOR_CEIL_BIT 0x2

/* This macros can be used to set the cell data on the map.
 *
 *  WALL CELL:
 *  The first bit on indicates wall cell, second byte contains the wall texture index,
 *  the remaining 2 upper bytes are left unused.*/
#define WALL(x) ((x << 8) | WALL_BIT) & 0x0000ff01
/*
 *  FLOOR AND CEILING CELL:
 *  The second bit on of the first byte indicates this is a floor/ceiling cell.
	the second byte contains the ceiling texture index
	the thirdy byte contains the floor texture index
	fourth byte is left unused
	the remaining 5 bits of the first byte are left unused and reserved for further usage.*/
#define FLCL(f, c) (f << 16 | c << 8 | FLOOR_CEIL_BIT) & 0x00ffff02

typedef struct{
	uint32_t * values;
	size_t w;
	size_t h;
	size_t cell_size;
	const SDL_Rect * viewport;
	const uint32_t * colors;
}Map;

void map_init(Map * map, uint32_t * values, size_t w, size_t h, const SDL_Rect * viewport);
void map_quit(Map * map);
void map_draw(const Map * map, SDL_Renderer * renderer, size_t window_w, size_t window_h);
void world_2_screen(const Map * map, const vec2f * world_pos, vec2i * screen);
