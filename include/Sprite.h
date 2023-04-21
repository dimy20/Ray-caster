#pragma once

#include <cstdint>
#include <vector>
#include "vec2.h"
#include <SDL2/SDL.h>

namespace rc{
	struct Core;
	struct Sprite{
		Sprite(const Vec2f& pos, int id, Core * core);
		Sprite& operator= (const Sprite& other);
		void draw(const SDL_Rect& dim, double dist_from_player) const;
		void update();

		public:
			Vec2f position;
			int texture_id;
			Core * m_core;
			double last_dist_to_player;
	};
}


