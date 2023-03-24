#pragma once

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <assert.h>

#include "map.h"

#define MAX(a,b) (a) > (b) ? (a) : (b)
#define MIN(a,b) (a) > (b) ? (b) : (a)

#define UNIMPLEMENTED do{ \
	fprintf(stderr, "UNIMPLEMENTED %s:%d\n", __FILE__, __LINE__); \
	exit(1); \
}while(0);

#define DIE(s) do{\
	fprintf(stderr, "Error: %s\n", s);\
	exit(1);\
}while(0);

#define KEYBOARD_MAX_KEYS 350
typedef enum{
	MAP_VIEWPORT,
	GAME_VIEWPORT,
	
	VIEWPORTS_NUM
}Viewport;

void engine_add_viewport(char * name, const SDL_Rect rect);

void engine_init(int w, int h);
void engine_run();
void engine_quit();
void engine_set_color(uint32_t color);
uint8_t engine_test_inputkey(SDL_Scancode key);

void engine_unpack_color(const uint32_t color, 
								uint8_t * r,
								uint8_t * g, 
								uint8_t * b, 
								uint8_t * a);

uint32_t engine_pack_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
void engine_draw_rect(SDL_Rect * rect);

int test_collision(const SDL_Rect a, const SDL_Rect b);
void engine_query_window(int * w, int * h);
