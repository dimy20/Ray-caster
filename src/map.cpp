#include "map.h"
#include "utils.h"
#include "RC_Engine.h"

#define BLACK 0x00000000
#define RED 0xff0000ff
#define GREEN 0x00ff00ff
#define BLUE 0x0000ffff

static uint32_t _colors[4] = {BLACK, RED, GREEN, BLUE};

rc::Map::Map(const uint32_t * _values, int map_w, int map_h){
	assert(w <= MAP_MAX_SIZE && h <= MAP_MAX_SIZE);

	values = std::vector<uint32_t>(_values, _values + (map_w * map_h));
	w = map_w;
	h = map_h;
	cell_size = CELL_SIZE;
	colors = _colors;
}

void rc::Map::draw(rc::Engine * engine, size_t window_w, size_t window_h){
	// map's cell size in screen space
	size_t cell_w_screen =  window_w / w;
	size_t cell_h_screen =  window_h / h;

	for(int y = 0; y < h; ++y){
		for(int x = 0; x < w; ++x){
			size_t screen_x = x * cell_w_screen;
			size_t screen_y = y * cell_h_screen;

			SDL_Rect rect = {(int)screen_x,
							 (int)screen_y,
							 (int)cell_w_screen,
							 (int)cell_h_screen};

			uint32_t cell_data = at(x, y);
			uint32_t color = colors[cell_data & WALL_BIT ? cell_data >> 8 : 0];

			engine->set_draw_color(color);
			RC_DIE(SDL_RenderFillRect(engine->renderer(), &rect) < 0, SDL_GetError());
		}
	}

	engine->set_draw_color(0xffffffff);
	// draw the grid
	for(int x = 0; x < w; ++x){
		size_t screen_x = (x * cell_w_screen);
		RC_DIE(SDL_RenderDrawLine(engine->renderer(),
						   (int)screen_x,
						   0,
						   (int)screen_x,
						   (int)window_h - 1) < 0, SDL_GetError());
	}

	for(int y = 0; y < h; ++y){
		size_t screen_y = y * cell_h_screen;
		RC_DIE(SDL_RenderDrawLine(engine->renderer(), 
					       0,
						   (int)screen_y,
						   (int)window_w - 1,
						   (int)screen_y) < 0, SDL_GetError());
	}

	//TODO: move this out of this function
	//engine->set_draw_color(0x00ff00ff);
	//for(int i = 0; i < map->sprites_len; i++){
	//	const vec2f * p = &map->sprites[i].position;
	//	vec2i screen;
	//	world_2_screen(map, p, &screen);
	//	SDL_Rect r = {screen.x, screen.y, 5, 5};
	//	SDL_RenderFillRect(engine->renderer(), &r);
	//}
}
