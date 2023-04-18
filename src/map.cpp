#include "map.h"
#include "utils.h"
#include "RC_Engine.h"

#define BLACK 0x00000000
#define RED 0xff0000ff
#define GREEN 0x00ff00ff
#define BLUE 0x0000ffff

static uint32_t colors[4] = {BLACK, RED, GREEN, BLUE};

void map_init(Map * map, uint32_t * values, size_t w, size_t h, const SDL_Rect * viewport){
	assert(map != NULL);
	assert(values != NULL);
	assert(w <= MAP_MAX_SIZE && h <= MAP_MAX_SIZE);

	map->values = static_cast<uint32_t *>(malloc(sizeof(uint32_t) * (w * h)));

	assert(map->values != NULL);

	map->w = w;
	map->h = h;

	map->cell_size = CELL_SIZE;
	map->viewport = viewport;
	memcpy(map->values, values, sizeof(uint32_t) * (w * h));
	map->colors = colors;

	memset(map->sprites, 0, sizeof(RC_Sprite));
	map->sprites_len = 0;
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
void map_draw(const Map * map, rc::Engine * engine, size_t window_w, size_t window_h){
	assert(map != NULL);

	// map's cell size in screen space
	size_t cell_w_screen =  window_w / map->w;
	size_t cell_h_screen =  window_h / map->h;

	for(int y = 0; y < map->h; ++y){
		for(int x = 0; x < map->w; ++x){
			size_t screen_x = x * cell_w_screen;
			size_t screen_y = y * cell_h_screen;

			SDL_Rect rect = {(int)screen_x,
							 (int)screen_y,
							 (int)cell_w_screen,
							 (int)cell_h_screen};

			uint32_t cell_data = map->values[y * map->w + x];
			uint32_t color = colors[cell_data & WALL_BIT ? cell_data >> 8 : 0];

			engine->set_draw_color(color);
			RC_DIE(SDL_RenderFillRect(engine->renderer(), &rect) < 0, SDL_GetError());
		}
	}

	engine->set_draw_color(0xffffffff);
	// draw the grid
	for(int x = 0; x < map->w; ++x){
		size_t screen_x = (x * cell_w_screen);
		RC_DIE(SDL_RenderDrawLine(engine->renderer(),
						   (int)screen_x,
						   0,
						   (int)screen_x,
						   (int)window_h - 1) < 0, SDL_GetError());
	}

	for(int y = 0; y < map->h; ++y){
		size_t screen_y = y * cell_h_screen;
		RC_DIE(SDL_RenderDrawLine(engine->renderer(), 
					       0,
						   (int)screen_y,
						   (int)window_w - 1,
						   (int)screen_y) < 0, SDL_GetError());
	}


	engine->set_draw_color(0x00ff00ff);
	for(int i = 0; i < map->sprites_len; i++){
		const vec2f * p = &map->sprites[i].position;
		vec2i screen;
		world_2_screen(map, p, &screen);
		SDL_Rect r = {screen.x, screen.y, 5, 5};
		SDL_RenderFillRect(engine->renderer(), &r);
	}
}

void map_quit(Map * map){
	assert(map != NULL);
	free(map->values);
}

void RC_Map_set_sprite(Map * map, int x, int y, int texture_id){
	if(map->sprites_len < MAX_SPRITES){
		RC_Sprite * sprite = &map->sprites[map->sprites_len++];
		sprite->position.x = x;
		sprite->position.y = y;
		sprite->texture_id = texture_id;
	}
}
