#include "RC_Engine.h"
#include "RC_Core.h"
#include <iostream>

#include "map.h"

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

#define TARGET_FPS 60
static const double target_time_per_frame = 1.0 / TARGET_FPS;
SDL_Texture * sprite_texture;
extern uint32_t sprite_pixels[PROJ_PLANE_W * PROJ_PLANE_H];

rc::Engine::Engine(int w, int h) : Core(PROJ_PLANE_W, PROJ_PLANE_H, 60.0){
	init(w, h);
}

rc::Engine::~Engine(){
	SDL_DestroyRenderer(m_renderer);
	SDL_DestroyWindow(m_window);
	SDL_Quit();
}

void rc::Engine::init_viewports(){
	m_viewports["map"] = {0, 0, 200, 200};
	m_viewports["scene"] = {0, 0, screen_w, screen_h};
}

void rc::Engine::load_textures(){
	m_resources->add_surface(FLOOR_TEXT, load_surface_RGBA(m_renderer, "./assets/floor.png"));
	m_resources->add_surface(SPACE_WALL_TEXT, load_surface_RGBA(m_renderer, "./assets/space_wall.png"));
	m_resources->add_surface(WOLF_WALL_TEXT, load_surface_RGBA(m_renderer, "./assets/wall.png"));
	m_resources->add_surface(CEILING_TEXT, m_resources->get_surface(WOLF_WALL_TEXT));
	m_resources->add_surface(BARREL_SPRITE, load_surface_RGBA(m_renderer, "./assets/barrel.png"));
}

void rc::Engine::init(int w, int h){
	screen_w = w;
	screen_h = h;

	SDL_Window * window;
	SDL_Renderer * renderer;

	int renderer_flags, window_flags;
	renderer_flags = SDL_RENDERER_ACCELERATED;
	window_flags = 0;

	RC_DIE(SDL_Init(SDL_INIT_VIDEO) < 0, SDL_GetError());

	window = SDL_CreateWindow("Shooter 01",
								 SDL_WINDOWPOS_UNDEFINED,
								 SDL_WINDOWPOS_UNDEFINED,
								 screen_w,
								 screen_h,
								 window_flags);

	RC_DIE(!window, SDL_GetError());

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	RC_DIE(!(renderer = SDL_CreateRenderer(window, -1, renderer_flags)), SDL_GetError());

	RC_DIE((IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) < 0), IMG_GetError());

	memset(&input.keyboard, 0, sizeof(uint32_t) * KEYBOARD_MAX_KEYS);
	
	time.prev_time = SDL_GetPerformanceCounter();
	time.frequency = SDL_GetPerformanceFrequency();
	time.delta_time = 0;

	m_window = window;
	m_renderer = renderer;
	m_running = true;

	init_viewports();
	load_textures();
		
	// initialize frame buffer to copy pixels from core
	RC_DIE(!(m_fbuffer_texture = SDL_CreateTexture(m_renderer,
								 SDL_PIXELFORMAT_RGBA8888,
								 SDL_TEXTUREACCESS_STREAMING,
							     PROJ_PLANE_W,
							     PROJ_PLANE_H)), SDL_GetError());




	RC_DIE(!(sprite_texture = SDL_CreateTexture(m_renderer, 
							                    SDL_PIXELFORMAT_RGBA8888, 
												SDL_TEXTUREACCESS_STREAMING, 
												PROJ_PLANE_W, PROJ_PLANE_H)), 
												SDL_GetError());
	RC_DIE(sprite_texture == NULL, SDL_GetError());
	SDL_SetTextureBlendMode(sprite_texture, SDL_BLENDMODE_BLEND);
}

void rc::Engine::do_keyup(const SDL_KeyboardEvent * e){
	if (e->repeat == 0 && e->keysym.scancode < KEYBOARD_MAX_KEYS){
		input.keyboard[e->keysym.scancode] = false;
	}
}

void rc::Engine::do_keydown(const SDL_KeyboardEvent * e){
	if (e->repeat == 0 && e->keysym.scancode < KEYBOARD_MAX_KEYS){
		input.keyboard[e->keysym.scancode] = true;
	}
}

void rc::Engine::do_input(){
	SDL_Event e;
	while (SDL_PollEvent(&e)){
		switch (e.type){
			case SDL_QUIT:
				m_running = false;
				break;
			case SDL_KEYDOWN: do_keydown(&e.key); break;
			case SDL_KEYUP: do_keyup(&e.key); break;
			default:
				break;
		}

	}

}

void rc::Engine::prepare_scene(){
	RC_DIE(SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 0) < 0, SDL_GetError());
	RC_DIE(SDL_RenderClear(m_renderer) < 0, SDL_GetError());
}

void rc::Engine::cap_fps(){
// cap fps to 60 fps
	uint64_t curr_time = SDL_GetPerformanceCounter();
	double time_diff = (double)((curr_time - time.prev_time));
	time.delta_time =  time_diff / time.frequency;

	if(time.delta_time < target_time_per_frame){
		double sleep_time = (target_time_per_frame - time.delta_time) * 1000.0;
		SDL_Delay(sleep_time);
	}

	time.prev_time = curr_time;
}

void rc::Engine::run(){
	while(m_running){
		do_input();

		cap_fps();

		prepare_scene();

		update();

		draw();

		SDL_RenderPresent(m_renderer);
	}
}

void rc::Engine::blit(SDL_Texture * t, SDL_Rect * src, SDL_Rect * dest){
	RC_DIE((SDL_RenderCopy(m_renderer, t, src, dest) < 0 ), SDL_GetError());
}

bool rc::box_collision(const SDL_Rect * r1, const SDL_Rect * r2){
	return (std::max(r1->x, r2->x)) < (std::min(r1->x + r1->w, r2->x + r2->w)) &&
		   (std::max(r1->y, r2->y)) < (std::min(r1->y + r1->h, r2->y + r2->h));
}

SDL_Texture * rc::load_texture(Engine * r2d, const char * filename){

	SDL_Texture * texture;
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Loading %s", filename);

	RC_DIE(!(texture = IMG_LoadTexture(r2d->renderer(), filename)), IMG_GetError());
	return texture;

}

SDL_Texture * rc::load_texture(Engine * r2d, const char * filename, uint32_t colorkey){
	SDL_Surface * surface;
	SDL_Texture * texture;

	RC_DIE(!(surface = IMG_Load(filename)), IMG_GetError());

	uint8_t r, g, b;
	unpack_color(colorkey, r, g, b);
	uint32_t key_color = SDL_MapRGB(surface->format, r, g, b);


	RC_DIE((SDL_SetColorKey(surface, SDL_TRUE, key_color) < 0), SDL_GetError());

	RC_DIE(!(texture = SDL_CreateTextureFromSurface(r2d->renderer(), surface)), SDL_GetError());

	SDL_FreeSurface(surface);

	return texture;

}

void rc::unpack_color(uint32_t color, uint8_t& r, uint8_t& g, uint8_t& b){
	r = static_cast<uint8_t>((color >> 24) & 0xff);
	g = static_cast<uint8_t>((color >> 16) & 0xff);
	b = static_cast<uint8_t>((color >> 8) & 0xff);
}

void rc::unpack_color(uint32_t color, uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& a){
	r = static_cast<uint8_t>((color >> 24) & 0xff);
	g = static_cast<uint8_t>((color >> 16) & 0xff);
	b = static_cast<uint8_t>((color >> 8) & 0xff);
	a = static_cast<uint8_t>(color & 0xff);
}

void rc::Engine::update(){
	m_player->update(this);
}

void rc::Engine::draw(){
	const uint32_t * fbuffer = render(DRAW_TEXT_MAPPED_WALLS);

	int pitch = sizeof(uint32_t) * PROJ_PLANE_W;

	RC_DIE(SDL_UpdateTexture(m_fbuffer_texture, NULL, fbuffer, pitch) < 0, SDL_GetError());
	RC_DIE(SDL_RenderSetViewport(m_renderer, &m_viewports["scene"]) < 0, SDL_GetError());
	RC_DIE(SDL_RenderCopy(m_renderer, m_fbuffer_texture, NULL, NULL) < 0, SDL_GetError());

//	draw the rays
	//SDL_SetRenderDrawColor(m_renderer, 0xff, 0xff, 0xff, 0xff);

	//RC_DIE(SDL_RenderSetViewport(m_renderer, &m_viewports["map"]) < 0, SDL_GetError());

	//SDL_Rect * map_vp = &m_viewports["map"];
	//map_draw(&map, this, (size_t)map_vp->w, (size_t)map_vp->h);
	//player_draw(&player, &map, m_renderer);

	//const vec2f * hits = get_hits();
	//for(size_t i = 0; i < PROJ_PLANE_W; i++){
	//	const vec2f * hit = &hits[i];
	//	assert(hit != NULL);

	//	if(hit->x != INT_MAX && hit->y != INT_MAX){
	//		vec2i player_screen, hit_screen;

	//		world_2_screen(&map, &player.position, &player_screen);
	//		vec2f hitf;

	//		hitf.x = hit->x;
	//		hitf.y = hit->y;

	//		world_2_screen(&map, &hitf, &hit_screen);

	//		RC_DIE(SDL_RenderDrawLine(m_renderer,
	//								  player_screen.x,
	//								  player_screen.y,
	//								  hit_screen.x,
	//								  hit_screen.y) < 0, SDL_GetError());
	//	}
	//}

	RC_DIE(SDL_RenderSetViewport(m_renderer, &m_viewports["scene"]) < 0, SDL_GetError());
	render_sprites(m_renderer);
	RC_DIE(SDL_UpdateTexture(sprite_texture, NULL, sprite_pixels, 4 * PROJ_PLANE_W) < 0,
			SDL_GetError());
	RC_DIE(SDL_RenderCopy(m_renderer, sprite_texture, NULL, NULL) < 0, SDL_GetError());
}

void rc::Engine::set_draw_color(uint32_t color){
	uint8_t r, g, b, a;
	unpack_color(color, r, g, b, a);
	RC_DIE(SDL_SetRenderDrawColor(m_renderer, r, g, b, a) < 0, SDL_GetError());
}

SDL_Surface * rc::load_surface_RGBA(SDL_Renderer * renderer, const std::string& filename){
	assert(renderer != NULL);
	SDL_Surface * surface, * s;

	RC_DIE(!(surface = IMG_Load(filename.c_str())), IMG_GetError());
	RC_DIE(!(s = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA8888, 0)), IMG_GetError());

	SDL_FreeSurface(surface);

	return s;
}

void rc::Engine::world_2_screen(const vec2f * world_pos, vec2i * screen) {
	size_t map_w = m_map->cell_size * m_map->w;
	size_t map_h = m_map->cell_size * m_map->h;

	const SDL_Rect& map_viewport = m_viewports["map"];

	float x_scale = static_cast<float>(map_viewport.w) / static_cast<float>(map_w);
	float y_scale = static_cast<float>(map_viewport.h) / static_cast<float>(map_h);

	screen->x = static_cast<int>(world_pos->x * x_scale);
	screen->y = static_cast<int>(world_pos->y * y_scale);
}
