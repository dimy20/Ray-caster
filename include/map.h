#pragma once

#include <cassert>
#include <vector>
#include "vec2.h"
#include "utils.h"

#include <SDL2/SDL.h>

#define MAP_MAX_SIZE 64
#define CELL_SIZE 64 // cube of dimensions 64 x 64 x 64
#define WALL_BIT 0x1
#define FLOOR_CEIL_BIT 0x2
#define MAX_SPRITES 128

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
#define FLCL(s, f, c) (s << 24 | f << 16 | c << 8 | FLOOR_CEIL_BIT) & 0xffffff02



namespace rc{
	struct Engine;

	struct Map{
		Map(){};
		Map(const uint32_t * values, int w, int h);
		void draw(rc::Engine * engine, size_t window_w, size_t window_h);
		inline uint32_t at(int x, int y) const { return values[y * w + x]; };

		public:
			int w;
			int h;
			size_t cell_size;
			std::vector<uint32_t> values;
			const uint32_t * colors;
	};
};























