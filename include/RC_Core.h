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
#include "Resources.h"
#include <unordered_map>

namespace rc{
	enum RenderFlag{
		DRAW_RAW_WALLS = 0x1,
		DRAW_TEXT_MAPPED_WALLS = 0x2,
	};

	struct Resources;

	struct Core{
		Core(size_t proj_plane_w, size_t proj_plane_h, double fov, rc::Resources * resources);
		~Core();
		void render_sprites(SDL_Renderer * renderer, const Map * map, const Player * player);
		const uint32_t * render(const Player * player, const Map * map, uint32_t flags);

		private:
			double find_h_intercept(const double ray_angle, const Player * player, const Map * map, vec2f * h_hit, vec2i * map_coords);

			double find_v_intercept(double ray_angle, const Player * player, const Map * map, vec2f * v_hit, vec2i * map_coords);

			void draw_textmapped_wall_slice(int texture_x, int slice_height, int screen_x, SDL_Surface * texture);

			void draw_wall_slice(int y_top, int y_bot, int x, uint32_t color);

			void draw_floor_slice(const Player * player, const Map * map, double ray_angle, int screen_x, int wall_bottom_y);

			void draw_celing_slice(const Player * player, const Map * map, double ray_angle, int screen_x, int wall_top);

			void sprite_screen_dimensions(int index, int screen_x, SDL_Rect * rect, const Player * player, const Map * map);

			void sprite_world_2_screen(const RC_Sprite * sprite, vec2i * screen_coords, const Player * player, int columns_per_angle);

			double perpendicular_distance(double viewing_angle, const vec2f * p, const vec2f * hit);
			constexpr bool column_in_bounds(int x) { return x >= 0 && x < m_proj_plane_w; };
			constexpr bool row_in_bounds(int y) { return y >= 0 && y < m_proj_plane_h; };


		private:
			int m_proj_plane_w;
			int m_proj_plane_h;
			int m_proj_plane_center;
			double m_angle_step;
			vec2f * m_hits;
			double * m_sprite_distance;
			rc::Resources * m_resources;

			struct Frame_buffer{
				Frame_buffer() {};
				Frame_buffer(int w, int h) : w(w), h(h) {
					pixels = new uint32_t[w * h];
					clear();
				};
				~Frame_buffer() { if(pixels) delete []pixels; };

				inline void clear() { memset(static_cast<void *>(pixels), 0, sizeof(uint32_t) * (w * h)); };
				inline void set_pixel(int x, int y, uint32_t color) { if(y < h && x < w) pixels[y * w + x] = color; };

				public:
					uint32_t * pixels;
					int w;
					int h;
			}m_fbuffer;

	};
}
