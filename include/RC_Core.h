#pragma once

#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>

#include <SDL2/SDL.h>

#include "map.h"
#include "player.h"
#include "rc_math.h"

typedef enum{
	DRAW_RAW_WALLS = 0x1,
	DRAW_TEXT_MAPPED_WALLS = 0x2,
}RC_RenderFlag;

typedef struct{
	size_t proj_plane_w;
	size_t proj_plane_h;
	size_t proj_plane_center;
	double angle_step;
	vec2f * hits;
	uint32_t * fbuffer;
	SDL_Surface ** textures;
	size_t textures_len;
	double * sprite_distance;
}Rc_context;

void RC_Core_init(size_t proj_plane_w, size_t proj_plane_h, double fov, SDL_Surface ** textures, size_t textures_len);
void RC_Core_render_sprites(SDL_Renderer * renderer, const Map * map, const Player * player);
const uint32_t * RC_Core_render(const Player * player, const Map * map, uint32_t flags);
void RC_Core_quit();
const vec2f * RC_Core_hits();


