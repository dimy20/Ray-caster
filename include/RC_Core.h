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

typedef struct{
	size_t proj_plane_w;
	size_t proj_plane_h;
	size_t proj_plane_center;
	vec2f * hits;
	uint32_t * fbuffer;
}Rc_context;

void RC_Core_init(size_t proj_plane_w, size_t proj_plane_h);
const uint32_t * RC_Core_render(const Player * player, const Map * map);
void RC_Core_quit();
const vec2f * RC_Core_hits();
