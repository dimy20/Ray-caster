#include "map.h"

#define BLACK 0x00000000
#define RED 0xff0000ff
#define GREEN 0x00ff00ff
#define BLUE 0x0000ffff

static uint32_t colors[4] = {BLACK, RED, GREEN, BLUE};

void map_init(Map * map, int * values, size_t w, size_t h, SDL_Rect viewport){
	assert(map != NULL);
	assert(values != NULL);

	map->values = malloc(sizeof(int) * (w * h));
	assert(map->values != NULL);

	map->w = w;
	map->h = h;

	map->viewport = viewport;
	map->cell_size = CELL_SIZE;

	memcpy(map->values, values, sizeof(int) * (w * h));
}

// void map_draw(const Map * map, Platform * platform)
void map_draw(const Map * map, SDL_Renderer * renderer){
	assert(map != NULL);
	assert(renderer != NULL);

	size_t window_w = (size_t)map->viewport.w;
	size_t window_h = (size_t)map->viewport.h;

	SDL_RenderSetViewport(renderer, &map->viewport);
	// map's cell size in screen space
	size_t cell_w_screen =  window_w / map->w;
	size_t cell_h_screen =  window_h / map->h;

	for(size_t y = 0; y < map->h; ++y){
		for(size_t x = 0; x < map->w; ++x){
			size_t screen_x = x * cell_w_screen;
			size_t screen_y = y * cell_h_screen;

			SDL_Rect rect = {(int)screen_x,
							 (int)screen_y,
							 (int)cell_w_screen,
							 (int)cell_h_screen};

			uint32_t color = colors[map->values[y * map->w + x]];

			engine_set_color(color);
			SDL_RenderFillRect(renderer, &rect);
		}
	}

	engine_set_color(0xffffffff);
	// draw the grid
	for(size_t x = 0; x < map->w; ++x){
		size_t screen_x = (x * cell_w_screen);
		SDL_RenderDrawLine(renderer,
						   (int)screen_x,
						   0,
						   (int)screen_x,
						   (int)window_h - 1);
	}

	for(size_t y = 0; y < map->h; ++y){
		size_t screen_y = y * cell_h_screen;
		SDL_RenderDrawLine(renderer, 
					       0,
						   (int)screen_y,
						   (int)window_w - 1,
						   (int)screen_y);
	}
}

void map_quit(Map * map){
	assert(map != NULL);
	free(map->values);
}
