#include "engine.h"
#include "RC_Core.h"
#include "map.h"

//TODO: this should live somewhere else, since users should be
//		able to set a map cell to the texture they want.
//		This is temporary
typedef enum{
	FLOOR_TEXT,
	SPACE_WALL_TEXT,
	WOLF_WALL_TEXT,
	CEILING_WALL_TEXT,
	BARREL_SPRITE,

	TEXTURES_NUM,
}TextureID;

SDL_Texture * sprite_texture;
extern uint32_t sprite_pixels[PROJ_PLANE_W * PROJ_PLANE_H];

typedef struct{
	/* Raycasting*/
	Player player;
	Map map;
	SDL_Texture * fbuffer_texture;
	/* INPUT */
	uint8_t keyboard[KEYBOARD_MAX_KEYS];

	/* SDL STUFF 
	 * TODO: maybe this should live on a lower layer, a platform layer??*/
	SDL_Renderer * renderer;

	/* STATE */
	bool running;

	/* Viewports */
	SDL_Rect viewports[VIEWPORTS_NUM];

	/*Window information*/
	SDL_Window * window;
	int w, h;

	SDL_Surface * textures[TEXTURES_NUM];

	/* TIME */
	double delta_time;
	uint32_t old_time;
}Engine;

static bool initialized = false;
Engine * engine = NULL;

uint32_t temp_map[8 * 8] = {
	WALL(1), WALL(1)      , WALL(1)      , WALL(1)      , WALL(1)      , WALL(1)      , WALL(1)      , WALL(1),
	WALL(1), FLCL(0, 0, 3), FLCL(0, 0, 3), FLCL(0, 0, 3), FLCL(0, 0, 3), FLCL(0, 0, 3), FLCL(0, 0, 3), WALL(1),
	WALL(1), FLCL(0, 0, 3), FLCL(0, 0, 3), WALL(1)   , FLCL(0, 0, 3), WALL(1)   , FLCL(0, 0, 3), WALL(1),
	WALL(1), FLCL(0, 0, 3), FLCL(0, 0, 3), WALL(1)   , WALL(1)   , WALL(1)   , FLCL(0, 0, 3), WALL(1),
	WALL(1), FLCL(0, 0, 3), FLCL(0, 0, 3), FLCL(0, 0, 3), FLCL(0, 0, 3), FLCL(0, 0, 3), FLCL(0, 0, 3), WALL(1),
	WALL(1), FLCL(0, 0, 3), FLCL(0, 0, 3), FLCL(0, 0, 3), WALL(1)   , FLCL(0, 0, 3), FLCL(0, 0, 3), WALL(1),
	WALL(1), FLCL(0, 0, 3), FLCL(0, 0, 3), FLCL(0, 0, 3), FLCL(0, 0, 3), FLCL(0, 0, 3), FLCL(0, 0, 3), WALL(1),
	WALL(1), WALL(1)   , WALL(1)   , WALL(1)   , WALL(1)   , WALL(1)   , WALL(1)   , WALL(1),
};

static void RC_Engine_init_viewport(SDL_Rect * vp, int x, int y, int w, int h){
	vp->x = x;
	vp->y = y;
	vp->w = w;
	vp->h = h;
}

static SDL_Surface * RC_Texture_load(SDL_Renderer * renderer, const char * filename){
	assert(renderer != NULL);
	SDL_Surface * surface, * s;

	if(!(surface = IMG_Load(filename))) DIE(IMG_GetError());
	if(!(s = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA8888, 0))) DIE(IMG_GetError());

	return s;
}

void RC_Engine_init(int w, int h){

	assert(!initialized);

	SDL_Window * window;
	SDL_Renderer * renderer;

	RC_DIE(SDL_Init(SDL_INIT_VIDEO) < 0);
	RC_DIE((window = SDL_CreateWindow("rc", 0, 0, w, h, 0)) == NULL);
	RC_DIE((renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED)) == NULL);
	if(!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) DIE(IMG_GetError());

	engine = malloc(sizeof(Engine));
	assert(engine != NULL);
	memset(engine, 0, sizeof(Engine));

	engine->window = window;
	engine->renderer = renderer;
	memset(engine->keyboard, 0, KEYBOARD_MAX_KEYS);

	engine->running = true;
	engine->w = w;
	engine->h = h;
	engine->old_time = SDL_GetTicks();
	/* Viewports */
	memset(engine->viewports, 0, sizeof(SDL_Rect) * VIEWPORTS_NUM);
	RC_Engine_init_viewport(&engine->viewports[MAP_VIEWPORT], 0, 0, 200, 200);
	RC_Engine_init_viewport(&engine->viewports[SCENE_VIEWPORT], 0, 0, w, h);

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
	RC_Map_set_sprite(&engine->map, 100, 100, BARREL_SPRITE);

	player_init(&engine->player, PROJ_PLANE_W);

	engine->textures[FLOOR_TEXT] = RC_Texture_load(engine->renderer,
														"./assets/floor.png");

	engine->textures[SPACE_WALL_TEXT] = RC_Texture_load(engine->renderer,
														"./assets/space_wall.png");
	engine->textures[WOLF_WALL_TEXT] = RC_Texture_load(engine->renderer,
														"./assets/wall.png");
	engine->textures[CEILING_WALL_TEXT] = engine->textures[WOLF_WALL_TEXT];
	engine->textures[BARREL_SPRITE] = RC_Texture_load(engine->renderer, "./assets/barrel.png");
	//SDL_Surface * s = 
	//SDL_SetColorKey(s, SDL_TRUE, 0x980088ff);

	RC_Core_init(PROJ_PLANE_W, PROJ_PLANE_H, engine->player.fov, engine->textures, TEXTURES_NUM);
	initialized = true;


	sprite_texture = SDL_CreateTexture(engine->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, PROJ_PLANE_W, PROJ_PLANE_H);
	RC_DIE(sprite_texture == NULL);
	SDL_SetTextureBlendMode(sprite_texture, SDL_BLENDMODE_BLEND);
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
	RC_DIE(SDL_SetRenderDrawColor(engine->renderer, 0, 0, 0, 0xff) < 0);
	RC_DIE(SDL_RenderClear(engine->renderer) < 0);
}

static void RC_Engine_present(){
	assert(engine != NULL);
	SDL_RenderPresent(engine->renderer);
}

inline uint32_t RC_Engine_pack_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a){
	return (uint32_t)(r << 24 | g << 16 | b << 8 | a);
}
static inline void RC_Engine_unpack_color(const uint32_t color, 
								uint8_t * r,
								uint8_t * g, 
								uint8_t * b, 
								uint8_t * a)
{
	*r = (uint8_t)((color >> 24) & 0xff);
	*g = (uint8_t)((color >> 16) & 0xff);
	*b = (uint8_t)((color >> 8) & 0xff);
	*a = (uint8_t)(color & 0xff);
}

static void RC_Engine_update(){
	player_update(&engine->player);
}

static void RC_Engine_draw(){
	const uint32_t * fbuffer = RC_Core_render(&engine->player, &engine->map,
											  DRAW_TEXT_MAPPED_WALLS);

	int pitch = sizeof(uint32_t) * PROJ_PLANE_W;

	RC_DIE(SDL_UpdateTexture(engine->fbuffer_texture, NULL, fbuffer, pitch) < 0);
	RC_DIE(SDL_RenderSetViewport(engine->renderer,&engine->viewports[SCENE_VIEWPORT]) < 0);
	RC_DIE(SDL_RenderCopy(engine->renderer, engine->fbuffer_texture, NULL, NULL) < 0);

//	draw the rays
	RC_Engine_set_color(0xffffffff);

	RC_DIE(SDL_RenderSetViewport(engine->renderer, &engine->viewports[MAP_VIEWPORT]) < 0);

	SDL_Rect * map_vp = &engine->viewports[MAP_VIEWPORT];
	map_draw(&engine->map, engine->renderer, (size_t)map_vp->w, (size_t)map_vp->h);
	player_draw(&engine->player, &engine->map, engine->renderer);

	const vec2f * hits = RC_Core_hits();
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


	RC_DIE(SDL_RenderSetViewport(engine->renderer, &engine->viewports[SCENE_VIEWPORT]) < 0);
	RC_Core_render_sprites(engine->renderer, &engine->map, &engine->player);
	RC_DIE(SDL_UpdateTexture(sprite_texture, NULL, sprite_pixels, 4 * PROJ_PLANE_W) < 0);
	RC_DIE(SDL_RenderCopy(engine->renderer, sprite_texture, NULL, NULL) < 0);
}

void RC_Engine_run(){
	while(engine->running){
		uint32_t now = SDL_GetTicks();
		engine->delta_time = (double)(now - engine->old_time) / 1000.0f;
		engine->old_time = now;

		RC_Engine_handle_input();

		RC_Engine_update();

		RC_Engine_clear();
		/* Render */
		RC_Engine_draw();

		RC_Engine_present();
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

