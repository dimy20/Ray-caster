#include "map.h"

#define BLACK 0x00000000
#define RED 0xff0000ff
#define GREEN 0x00ff00ff
#define BLUE 0x0000ffff

uint32_t colors[4] = {BLACK, RED, GREEN, BLUE};

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

	int window_w, window_h;
	window_w = map->viewport.w;
	window_h = map->viewport.h;

	SDL_RenderSetViewport(renderer, &map->viewport);
	// map's cell size in screen space
	int cell_w_screen =  window_w / map->w;
	int cell_h_screen =  window_h / map->h;

	for(int y = 0; y < map->h; ++y){
		for(int x = 0; x < map->w; ++x){
			int screen_x = x * cell_w_screen;
			int screen_y = y * cell_h_screen;

			SDL_Rect rect = {screen_x,
							 screen_y,
							 cell_w_screen,
							 cell_h_screen};

			uint32_t color = colors[map->values[y * map->w + x]];

			engine_set_color(color);
			SDL_RenderFillRect(renderer, &rect);
		}
	}

	engine_set_color(0xffffffff);
	// draw the grid
	for(size_t x = 0; x < map->w; ++x){
		int screen_x = (x * cell_w_screen);
		SDL_RenderDrawLine(renderer,
						   screen_x,
						   0,
						   screen_x,
						   window_h - 1);
	}

	for(size_t y = 0; y < map->h; ++y){
		int screen_y = y * cell_h_screen;
		SDL_RenderDrawLine(renderer, 
					       0,
						   screen_y,
						   window_w - 1,
						   screen_y);
	}
}

void map_quit(Map * map){
	assert(map != NULL);
	free(map->values);
}
