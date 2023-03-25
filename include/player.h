#pragma once
#include <SDL2/SDL.h>

#include "engine.h"
#include "rc_math.h"
#include "map.h"

#include <math.h>

#define FOV 60
#define PLAYER_HEIGHT 32
#define PLAYER_VIEWING_ANGLE 45

typedef struct{
	double dist_from_proj_plane;
	int height;
	double viewing_angle;
	double fov;
	vec2f position;

	float speed;
	float rotation_speed;
}Player;

void player_draw(const Player * player,
				 const Map * map,
				 SDL_Renderer * renderer);

void player_init(Player * player, size_t projection_plane_w);

void player_update(Player * player);
