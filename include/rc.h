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
	size_t projection_plane_w;
	size_t projection_plane_h;
	vec2i  * h_hits;
}Rc_context;

void rc_init(Player * player);
void rc_cast(const Player * player, const Map * map);
void rc_draw_rays(SDL_Renderer * renderer, const Player * player, const Map * map);
void rc_quit();
