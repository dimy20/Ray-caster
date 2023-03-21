#include "map.h"
#include <stdio.h>

#define BLACK 0x00000000
#define RED 0xff0000ff
#define GREEN 0x00ff00ff
#define BLUE 0x0000ffff

uint32_t colors[4] = {BLACK, RED, GREEN, BLUE};

int map[MAP_W][MAP_H] = {
	{1, 1, 1, 1, 1, 1, 1, 1},
	{1, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 2, 0, 2, 0, 1},
	{1, 0, 0, 2, 2, 2, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 3, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 1},
	{1, 1, 1, 1, 1, 1, 1, 1},
};

void map_init(){
	SDL_Rect vp = {0, 0, 300, 300};
	engine->viewports[MAP_VIEWPORT] = vp;
}

void map_draw(){
	int window_w, window_h;
	window_w = engine->viewports[MAP_VIEWPORT].w;
	window_h = engine->viewports[MAP_VIEWPORT].h;

	SDL_RenderSetViewport(engine->renderer,
						  &engine->viewports[MAP_VIEWPORT]);

	// map's cell size in screen space
	int cell_w_screen =  window_w / MAP_W;
	int cell_h_screen =  window_h / MAP_H;

	for(int map_y = 0; map_y < MAP_H; ++map_y){
		for(int map_x = 0; map_x < MAP_W; ++map_x){
			int screen_x = map_x * cell_w_screen;
			int screen_y = map_y * cell_h_screen;

			SDL_Rect rect = {screen_x,
							 screen_y,
							 cell_w_screen,
							 cell_h_screen};

			uint32_t color = colors[map[map_x][map_y]];

			engine_set_color(color);
			SDL_RenderFillRect(engine->renderer, &rect);
		}
	}

	engine_set_color(0xffffffff);
	// draw the grid
	for(size_t x = 0; x < MAP_W; ++x){
		int screen_x = (x * cell_w_screen);
		SDL_RenderDrawLine(engine->renderer,
						   screen_x,
						   0,
						   screen_x,
						   window_h - 1);
	}

	for(size_t y = 0; y < MAP_H; ++y){
		int screen_y = y * cell_h_screen;
		SDL_RenderDrawLine(engine->renderer, 0, screen_y, window_w - 1, screen_y);
	}
}
