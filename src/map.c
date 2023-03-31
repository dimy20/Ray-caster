#include "map.h"

#define BLACK 0x00000000
#define RED 0xff0000ff
#define GREEN 0x00ff00ff
#define BLUE 0x0000ffff

static uint32_t colors[4] = {BLACK, RED, GREEN, BLUE};

void map_init(Map * map, uint32_t * values, size_t w, size_t h, const SDL_Rect * viewport){
	assert(map != NULL);
	assert(values != NULL);

	map->values = malloc(sizeof(uint32_t) * (w * h));

	assert(map->values != NULL);

	map->w = w;
	map->h = h;

	map->cell_size = CELL_SIZE;
	map->viewport = viewport;
	memcpy(map->values, values, sizeof(uint32_t) * (w * h));
	map->colors = colors;
}

void world_2_screen(const Map * map, const vec2f * world_pos, vec2i * screen){
	size_t map_w = map->cell_size * map->w;
	size_t map_h = map->cell_size * map->h;

	float x_scale = (float)(map->viewport->w) / (float)(map_w);
	float y_scale = (float)(map->viewport->h) / (float)(map_h);

	screen->x = (int)(world_pos->x * x_scale);
	screen->y = (int)(world_pos->y * y_scale);
}

// void map_draw(const Map * map, Platform * platform)
void map_draw(const Map * map, SDL_Renderer * renderer, size_t window_w, size_t window_h){
	assert(map != NULL);
	assert(renderer != NULL);

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

			uint32_t cell_data = map->values[y * map->w + x];
			uint32_t color = colors[cell_data & WALL_BIT ? cell_data >> 8 : 0];

			RC_Engine_set_color(color);
			RC_DIE(SDL_RenderFillRect(renderer, &rect) < 0);
		}
	}

	RC_Engine_set_color(0xffffffff);
	// draw the grid
	for(size_t x = 0; x < map->w; ++x){
		size_t screen_x = (x * cell_w_screen);
		RC_DIE(SDL_RenderDrawLine(renderer,
						   (int)screen_x,
						   0,
						   (int)screen_x,
						   (int)window_h - 1) < 0);
	}

	for(size_t y = 0; y < map->h; ++y){
		size_t screen_y = y * cell_h_screen;
		RC_DIE(SDL_RenderDrawLine(renderer, 
					       0,
						   (int)screen_y,
						   (int)window_w - 1,
						   (int)screen_y) < 0);
	}
}

void map_quit(Map * map){
	assert(map != NULL);
	free(map->values);
}
