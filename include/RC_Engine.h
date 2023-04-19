#pragma once

#include <unordered_map>
#include <string>

#include "utils.h"
#include "player.h"
#include "RC_Core.h"

#define KEYBOARD_MAX_KEYS 350

#define PROJ_PLANE_W 800
#define PROJ_PLANE_H 600

namespace rc{
	enum TextureID{
		FLOOR_TEXT,
		SPACE_WALL_TEXT,
		WOLF_WALL_TEXT,
		CEILING_TEXT,
		BARREL_SPRITE,

		TEXTURES_NUM,
	};

	struct Engine : public Core{
		Engine(int w, int h);
		~Engine();
		constexpr SDL_Renderer * renderer(){ return m_renderer; };
		void run();
		void blit(SDL_Texture * t, SDL_Rect * src, SDL_Rect * dest);
		void set_draw_color(uint32_t color);

		private:
			void init(int w, int h);
			void do_keyup(const SDL_KeyboardEvent * e);
			void do_keydown(const SDL_KeyboardEvent * e);
			void do_input();
			void prepare_scene();
			void cap_fps();
			void update();
			void draw();

			void load_textures();
			void init_viewports();
		
			SDL_Window * m_window;
			SDL_Renderer * m_renderer;
			bool m_running;
			std::unordered_map<std::string, SDL_Rect> m_viewports;
			SDL_Texture * m_fbuffer_texture;

			Player player;
			Map map;

		public:
			int screen_w;
			int screen_h;

			struct{
				uint64_t prev_time;
				uint64_t frequency;
				double delta_time;
			}time;

			struct{
				uint32_t keyboard[KEYBOARD_MAX_KEYS];
			}input;
	};

	void unpack_color(uint32_t color, uint8_t& r, uint8_t& g, uint8_t& b);
	void unpack_color(uint32_t color, uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& a);
	bool box_collision(const SDL_Rect * r1, const SDL_Rect * r2);
	SDL_Texture * load_texture(Engine * r2d, const char * filename);
	SDL_Texture * load_texture(Engine * r2d, const char * filename, uint32_t colorkey);
	SDL_Surface * load_surface_RGBA(SDL_Renderer * renderer, const std::string& filename);
}
