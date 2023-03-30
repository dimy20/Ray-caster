#pragma once

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <limits.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define PROJ_PLANE_W 600
#define PROJ_PLANE_H 400

#define UNIMPLEMENTED do{ \
	fprintf(stderr, "UNIMPLEMENTED %s:%d\n", __FILE__, __LINE__); \
	exit(1); \
}while(0);

#define DIE(s) do{ \
	fprintf(stderr, "Error: %s at %s:%d\n", s, __FILE__, __LINE__); \
	exit(1);										\
}while(0);

#define RC_DIE(v) do{ \
	if(v){											\
		fprintf(stderr, "Error: %s at %s:%d\n", SDL_GetError(), __FILE__, __LINE__); \
		exit(1);										\
	}													\
}while(0);
#define KEYBOARD_MAX_KEYS 350

typedef enum{
	MAP_VIEWPORT,
	SCENE_VIEWPORT,
	
	VIEWPORTS_NUM
}Viewport;

void engine_add_viewport(char * name, const SDL_Rect rect);

void RC_Engine_init(int w, int h);
void RC_Engine_run();
void RC_Engine_quit();
void RC_Engine_set_color(uint32_t color);
uint8_t RC_Engine_test_inputkey(SDL_Scancode key);
double RC_Engine_deltatime();
