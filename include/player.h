#pragma once

#include <SDL2/SDL.h>
#include "map.h"

#define FOV 60
#define PLAYER_HEIGHT 32
#define PLAYER_VIEWING_ANGLE 45

namespace rc{
	struct Engine;

	struct Player{
		Player() {};
		Player(int projection_plane_w);
		void draw(const Map * map, Engine * engine);
		void update(const rc::Engine * engine);

		public:
			double dist_from_proj_plane;
			int height;
			double viewing_angle;
			double fov;
			Vec2f position;
			double speed;
			double rotation_speed;
	};
}
