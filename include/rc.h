#pragma once

#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>

#include "engine.h"
#include "map.h"
#include "rc_math.h"
#include "player.h"

#define PROJECTION_PLANE_W 320
#define PROJECTION_PLANE_H 200

typedef struct{
	size_t proj_plane_w;
	size_t proj_plane_h;
	size_t proj_plane_center;
	vec2f * h_hits;
	vec2f * v_hits;
	vec2f * hits;
	uint32_t * fbuffer;
	SDL_Rect viewport;
	SDL_Texture * fbuffer_texture;
}Rc_context;

void rc_init(SDL_Renderer * renderer, Player * player, SDL_Rect viewport);
void rc_cast(SDL_Renderer * renderer, const Player * player, const Map * map);
void rc_draw_rays(SDL_Renderer * renderer, const Player * player, const Map * map);
void rc_quit();
