#include "engine.h"
#include "RC_Core.h"
#include "map.h"

typedef struct{
	/* Raycasting*/
	Player player;
	Map map;
	SDL_Texture * fbuffer_texture;
	/* INPUT */
	uint8_t keyboard[KEYBOARD_MAX_KEYS];

	/* SDL STUFF 
	 * TODO: make this should live on a lower layer, a platform layer??*/
	SDL_Renderer * renderer;

	/* STATE */
	bool running;

	/* Viewports */
	SDL_Rect viewports[VIEWPORTS_NUM];

	/*Window information*/
	SDL_Window * window;
	int w, h;

	/* TIME */
	double delta_time;
	uint32_t old_time, now_time;
}Engine;

static bool initialized = false;
Engine * engine = NULL;

int temp_map[8 * 8] = {
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 2, 0, 2, 0, 1,
	1, 0, 0, 2, 2, 2, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 3, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 1,
	1, 1, 1, 1, 1, 1, 1, 1,
};

static void RC_Engine_init_viewport(SDL_Rect * vp, int x, int y, int w, int h){
	vp->x = x;
	vp->y = y;
	vp->w = w;
	vp->h = h;
}

void RC_Engine_init(int w, int h){
	assert(!initialized);

	SDL_Window * window;
	SDL_Renderer * renderer;

	RC_DIE(SDL_Init(SDL_INIT_EVERYTHING) < 0);
	RC_DIE((window = SDL_CreateWindow("rc", 0, 0, w, h, 0)) == NULL);
	RC_DIE((renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED)) == NULL);

	engine = malloc(sizeof(Engine));
	assert(engine != NULL);
	memset(engine, 0, sizeof(Engine));

	engine->window = window;
	engine->renderer = renderer;
	engine->old_time = engine->now_time = 0;
	memset(engine->keyboard, 0, KEYBOARD_MAX_KEYS);

	engine->running = true;
	engine->w = w;
	engine->h = h;

	/* Viewports */
	memset(engine->viewports, 0, sizeof(SDL_Rect) * VIEWPORTS_NUM);
	RC_Engine_init_viewport(&engine->viewports[MAP_VIEWPORT], 0, 0, 300, 300);
	RC_Engine_init_viewport(&engine->viewports[SCENE_VIEWPORT], 300 + 1, 0,
			             PROJ_PLANE_W, PROJ_PLANE_H);

	/*This texture will be updated with the frame buffer that is drawn by
	 * the rc core algorithm each frame*/
	SDL_Texture * texture;
	RC_DIE((texture = SDL_CreateTexture(engine->renderer,
										SDL_PIXELFORMAT_RGBA8888,
										SDL_TEXTUREACCESS_STREAMING,
										PROJ_PLANE_W,
										PROJ_PLANE_H)) == NULL);

	engine->fbuffer_texture = texture;

	map_init(&engine->map, temp_map, 8, 8, &engine->viewports[MAP_VIEWPORT]);
	player_init(&engine->player, PROJ_PLANE_W);

	RC_Core_init(PROJ_PLANE_W, PROJ_PLANE_H);

	initialized = true;
}

void RC_Engine_quit(){
	assert(engine != NULL);
	RC_Core_quit();

	SDL_DestroyRenderer(engine->renderer);
	SDL_DestroyWindow(engine->window);

	map_quit(&engine->map);

	free(engine);

	SDL_Quit();
}

inline static void RC_Engine_keydown(const SDL_KeyboardEvent * e){
	assert(engine != NULL);
	SDL_Scancode code = e->keysym.scancode;
	if(e->repeat == 0 && code < KEYBOARD_MAX_KEYS){
		engine->keyboard[code] = 1;
	}

}

inline static void RC_Engine_keyup(const SDL_KeyboardEvent * e){
	assert(engine != NULL);
	SDL_Scancode code = e->keysym.scancode;
	if(e->repeat == 0 && code < KEYBOARD_MAX_KEYS){
		engine->keyboard[code] = 0;
	}
}

void RC_Engine_handle_input(){
	assert(engine != NULL);
	SDL_Event e;
	while(SDL_PollEvent(&e)){
		switch(e.type){
			case SDL_QUIT:
				engine->running = false;
				break;
			case SDL_KEYDOWN:
				RC_Engine_keydown(&e.key);
				break;
			case SDL_KEYUP:
				RC_Engine_keyup(&e.key);
				break;
			default:
				break;
		}
	}
}

static void RC_Engine_clear(){
	RC_DIE(SDL_SetRenderDrawColor(engine->renderer, 0, 0, 0, 0) < 0);
	RC_DIE(SDL_RenderClear(engine->renderer) < 0);
}

static void RC_Engine_present(){
	assert(engine != NULL);
	SDL_RenderPresent(engine->renderer);
}

inline uint32_t RC_Engine_pack_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a){
	return r << 24 | g << 16 | b << 8 | a;
}
static inline void RC_Engine_unpack_color(const uint32_t color, 
								uint8_t * r,
								uint8_t * g, 
								uint8_t * b, 
								uint8_t * a)
{
	*r = (color >> 24) & 0xff;
	*g = (color >> 16) & 0xff;
	*b = (color >> 8) & 0xff;
	*a = color & 0xff;
}

void RC_Engine_cap_framerate(){
	assert(engine != NULL);
	static float remainder = 0.0f;
	uint64_t wait;

	wait = 16 + remainder;

	remainder -= (int)remainder;

	engine->now_time = SDL_GetTicks();
	uint64_t delta = engine->now_time - engine->old_time;

	wait -= delta;

	wait = MAX(wait, 1UL);

	SDL_Delay(wait);

	remainder += 0.667f;

	engine->old_time = SDL_GetTicks();
}

static void RC_Engine_update(){
	player_update(&engine->player);
}

static void RC_Engine_draw(){
	RC_DIE(SDL_RenderSetViewport(engine->renderer,
					             &engine->viewports[MAP_VIEWPORT]) < 0);

	SDL_Rect * map_vp = &engine->viewports[MAP_VIEWPORT];
	map_draw(&engine->map, engine->renderer, map_vp->w, map_vp->h);
	player_draw(&engine->player, &engine->map, engine->renderer);

	const uint32_t * fbuffer = RC_Core_render(&engine->player, &engine->map);

	size_t pitch = sizeof(uint32_t) * PROJ_PLANE_W;

	RC_DIE(SDL_UpdateTexture(engine->fbuffer_texture, NULL, fbuffer, pitch) < 0);
	RC_DIE(SDL_RenderSetViewport(engine->renderer,&engine->viewports[SCENE_VIEWPORT]) < 0);
	RC_DIE(SDL_RenderCopy(engine->renderer, engine->fbuffer_texture, NULL, NULL) < 0);

	//draw the rays
	RC_Engine_set_color(0xffffffff);

	const vec2f * hits = RC_Core_hits();

	RC_DIE(SDL_RenderSetViewport(engine->renderer, &engine->viewports[MAP_VIEWPORT]) < 0);

	for(size_t i = 0; i < PROJ_PLANE_W; i++){
		const vec2f * hit = &hits[i];
		assert(hit != NULL);

		if(hit->x != INT_MAX && hit->y != INT_MAX){
			vec2i player_screen, hit_screen;

			world_2_screen(&engine->map, &engine->player.position, &player_screen);
			vec2f hitf;

			hitf.x = hit->x;
			hitf.y = hit->y;

			world_2_screen(&engine->map, &hitf, &hit_screen);

			RC_DIE(SDL_RenderDrawLine(engine->renderer,
									  player_screen.x,
									  player_screen.y,
									  hit_screen.x,
									  hit_screen.y) < 0);
		}
	}
}

void RC_Engine_run(){
	engine->old_time = SDL_GetTicks();

	uint32_t old = SDL_GetTicks();

	while(engine->running){
		RC_Engine_clear();

		RC_Engine_handle_input();
		/* update */
		RC_Engine_update();

		/* Render */
		RC_Engine_draw();

		uint32_t now = SDL_GetTicks();
		engine->delta_time = (now - old) / 1000.0f;
		old = now;

		/* End Render */
		RC_Engine_present();

		RC_Engine_cap_framerate();

	}
}

uint8_t RC_Engine_test_inputkey(SDL_Scancode key){
	assert(engine != NULL);
	return engine->keyboard[key];
}

void RC_Engine_set_color(uint32_t color){
	assert(engine != NULL);
	uint8_t r, g, b, a;
	RC_Engine_unpack_color(color, &r, &g, &b, &a);

	RC_DIE(SDL_SetRenderDrawColor(engine->renderer, r, g, b, a) < 0);
}

double RC_Engine_deltatime(){
	return engine->delta_time;
}

