#include "engine.h"
#include "rc.h"

typedef struct{
	/* Raycasting*/
	Player player;

	Map map;
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
	uint64_t delta_time;
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



int test_collision(const SDL_Rect a, const SDL_Rect b){
	return (a.x < b.x + b.w &&
			a.x + a.w > b.x &&
			a.y < b.y + b.h &&
			a.y + a.h > b.y);
};

void engine_init(int w, int h){
	assert(!initialized);

	SDL_Window * window;
	SDL_Renderer * renderer;

	if(SDL_Init(SDL_INIT_EVERYTHING) < 0) DIE(SDL_GetError());

	window = SDL_CreateWindow("rc", 0, 0, w, h, 0);
	if(!window) DIE(SDL_GetError());

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if(!renderer) DIE(SDL_GetError());

	engine = malloc(sizeof(Engine));
	assert(engine != NULL);

	engine->window = window;
	engine->renderer = renderer;
	engine->old_time = engine->now_time = engine->delta_time = 0;
	memset(engine->keyboard, 0, KEYBOARD_MAX_KEYS);

	engine->running = true;
	engine->w = w;
	engine->h = h;

	memset(engine->viewports, 0, sizeof(SDL_Rect) * VIEWPORTS_NUM);

	SDL_Rect viewport = {0, 0, 300, 300};
	map_init(&engine->map, temp_map, 8, 8, viewport);

	rc_init(&engine->player);

	initialized = true;
}

void engine_quit(){
	assert(engine != NULL);
	SDL_DestroyRenderer(engine->renderer);
	SDL_DestroyWindow(engine->window);
	SDL_Quit();

	rc_quit();
	map_quit(&engine->map);

	free(engine);
}

inline static void engine_keydown(const SDL_KeyboardEvent * e){
	assert(engine != NULL);
	SDL_Scancode code = e->keysym.scancode;
	if(e->repeat == 0 && code < KEYBOARD_MAX_KEYS){
		engine->keyboard[code] = 1;
	}

}

inline static void engine_keyup(const SDL_KeyboardEvent * e){
	assert(engine != NULL);
	SDL_Scancode code = e->keysym.scancode;
	if(e->repeat == 0 && code < KEYBOARD_MAX_KEYS){
		engine->keyboard[code] = 0;
	}
}

void engine_handle_input(){
	assert(engine != NULL);
	SDL_Event e;
	while(SDL_PollEvent(&e)){
		switch(e.type){
			case SDL_QUIT:
				engine->running = false;
				break;
			case SDL_KEYDOWN:
				engine_keydown(&e.key);
				break;
			case SDL_KEYUP:
				engine_keyup(&e.key);
				break;
			default:
				break;
		}
	}
}

static void engine_clear(){
	SDL_SetRenderDrawColor(engine->renderer, 0, 0, 0, 0);
	SDL_RenderClear(engine->renderer);
}

static void engine_present(){
	assert(engine != NULL);
	SDL_RenderPresent(engine->renderer);
}


inline uint32_t engine_pack_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a){
	return r << 24 | g << 16 | b << 8 | a;
}
void engine_unpack_color(const uint32_t color, 
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

void engine_cap_framerate(){
	assert(engine != NULL);
	static float remainder = 0.0f;
	uint64_t wait;

	wait = 16 + remainder;

	remainder -= (int)remainder;

	engine->now_time = SDL_GetTicks();
	engine->delta_time = engine->now_time - engine->old_time;

	wait -= engine->delta_time;

	wait = MAX(wait, 1UL);

	SDL_Delay(wait);

	remainder += 0.667f;

	engine->old_time = SDL_GetTicks();
}

static void engine_update(){
	player_update(&engine->player);
}
void engine_run(){
	engine->old_time = SDL_GetTicks();

	while(engine->running){
		engine_clear();

		engine_handle_input();
		/* update */
		engine_update();

		/* Render */
		map_draw(&engine->map, engine->renderer);
		player_draw(&engine->player, &engine->map, engine->renderer);

		rc_cast(engine->renderer, &engine->player, &engine->map);
		rc_draw_rays(engine->renderer, &engine->player, &engine->map);

		/* End Render */
		engine_present();

		engine_cap_framerate();
	}
}

uint8_t engine_test_inputkey(SDL_Scancode key){
	assert(engine != NULL);
	return engine->keyboard[key];
}

void engine_set_color(uint32_t color){
	assert(engine != NULL);
	uint8_t r, g, b, a;
	engine_unpack_color(color, &r, &g, &b, &a);

	if(SDL_SetRenderDrawColor(engine->renderer, r, g, b, a) < 0){
		DIE(SDL_GetError());
	}
}

void engine_draw_rect(SDL_Rect * rect){
	assert(engine != NULL);
	SDL_RenderDrawRect(engine->renderer, rect);
}

void engine_query_window(int * w, int * h){
	assert(engine != NULL);
	*w = engine->w;
	*h = engine->h;
}
