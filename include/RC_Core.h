#pragma once

#include <unordered_map>
#include <vector>
#include <memory>
#include <climits>
#include <SDL2/SDL.h>

#include "map.h"
#include "player.h"
#include "utils.h"
#include "Resources.h"


namespace rc{
	enum RenderFlag{
		DRAW_RAW_WALLS = 0x1,
		DRAW_TEXT_MAPPED_WALLS = 0x2,
	};

	struct Resources;

	struct Core{
		Core(size_t proj_plane_w, size_t proj_plane_h, double fov);
		~Core();
		void render_sprites(SDL_Renderer * renderer);
		const uint32_t * render(uint32_t flags);
		constexpr const std::vector<Vec2f>& hits() const { return m_hits; };

		private:
			double find_h_intercept(double ray_angle, Vec2f& h_hit, Vec2i& map_coords);
			double find_v_intercept(double ray_angle, Vec2f& v_hit, Vec2i& map_coords);

			void draw_textmapped_wall_slice(int texture_x, int slice_height, int screen_x, SDL_Surface * texture);

			void draw_wall_slice(int y_top, int y_bot, int x, uint32_t color);

			void draw_floor_slice(double ray_angle, int screen_x, int wall_bottom_y);

			void draw_celing_slice(double ray_angle, int screen_x, int wall_top);

			SDL_Rect sprite_screen_dimensions(int index, int screen_x);

			Vec2i sprite_world_2_screen(const RC_Sprite& sprite);

			double perpendicular_distance(double viewing_angle, const Vec2f& p, const Vec2f& hit);

			constexpr bool column_in_bounds(int x) const { return x >= 0 && x < m_proj_plane_w; };
			constexpr bool row_in_bounds(int y) const { return y >= 0 && y < m_proj_plane_h; };


		protected:
			rc::Resources * m_resources;
			std::unique_ptr<Player> m_player;
			std::unique_ptr<Map> m_map;

		private:
			int m_proj_plane_w;
			int m_proj_plane_h;
			int m_proj_plane_center;
			double m_angle_step;
			std::vector<Vec2f> m_hits;
			double * m_sprite_distance;

			/*These are values that are used repeatedly throughout Core for other calculations.
			 *However they can be known at start up, so they are computed once and kept in this
			 struct for access.*/
			struct{
				double half_fov;
				int columns_per_angle;  // constant value used in sprite_world_2_screen conversion.
				double cell_size_times_dist; // constant value used to calculate a wall slice height.
				double pheight_times_distplane;  /* Used to figure out the straight distance to a point P at the ceiling or floor.*/
			}m_constants;

			struct Frame_buffer{
				Frame_buffer() {};
				Frame_buffer(int w, int h) : w(w), h(h) { pixels.resize(w * h, 0); };
				inline void clear() { std::fill(pixels.begin(), pixels.end(), 0); };
				inline void set_pixel(int x, int y, uint32_t color) { if(y < h && x < w) pixels[y * w + x] = color; };

				public:
					std::vector<uint32_t> pixels;
					int w;
					int h;
			}m_fbuffer;
	};
}
