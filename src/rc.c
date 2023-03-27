#include "rc.h"
#include <math.h>
#include <stdbool.h>
#include <limits.h>

#define BLACK 0x00000000
#define RED 0xff0000ff
#define GREEN 0x00ff00ff
#define BLUE 0x0000ffff

static uint32_t colors[4] = {BLACK, RED, GREEN, BLUE};

static Rc_context * rctx;
static bool initted = false;

void world_2_screen(const vec2f * world_pos, vec2i * screen, const Map * map){
	size_t map_w = map->cell_size * map->w;
	size_t map_h = map->cell_size * map->h;

	float x_scale = (map->viewport.w) / (float)(map_w);
	float y_scale = (map->viewport.h) / (float)(map_h);

	screen->x = world_pos->x * x_scale;
	screen->y = world_pos->y * y_scale;
}

void rc_init(SDL_Renderer * renderer, Player * player, size_t proj_plane_w, size_t proj_plane_h){
	if(initted){
		//TODO: Log error
		return;
	}
	rctx = malloc(sizeof(Rc_context));
	assert(rctx != NULL);

	rctx->proj_plane_w = proj_plane_w;
	rctx->proj_plane_h = proj_plane_h;
	rctx->proj_plane_center = proj_plane_h / 2;

	rctx->hits = malloc(sizeof(vec2f) * rctx->proj_plane_w);
	memset(rctx->hits, 0, sizeof(vec2f) * rctx->proj_plane_w);

	initted = true;

	player_init(player, rctx->proj_plane_w);

	size_t dim = rctx->proj_plane_w * rctx->proj_plane_h;
	rctx->fbuffer = malloc(sizeof(uint32_t) * dim);
	assert(rctx->fbuffer != NULL);
}

void rc_clear_buffer(){
	size_t dim = rctx->proj_plane_w * rctx->proj_plane_h;
	memset(rctx->fbuffer, 0, sizeof(uint32_t) * dim);
}

void rc_quit(){
	assert(rctx != NULL);
	free(rctx);
}

vec2f cast_horizontal_intercept(const float ray_angle,
									  const Player * player,
									  const Map * map, vec2i * map_coords){
	vec2f h_hit;

	int step_y;
	double delta_step_x;

	/*Cell grid position for the current player's position.
	 * We actually round this down because we're actually looking for h_hit
	 * x and y positions, not the player's.*/
	int map_y = (int)(player->position.y / map->cell_size);

	if(ray_angle > 0.0 && ray_angle < 180.0){
		h_hit.y = map_y * (map->cell_size) - 1;

		int dy = player->position.y - h_hit.y;
		double dx = dy / tan(TO_RAD(ray_angle));

		h_hit.x = player->position.x + dx;

		step_y = -map->cell_size;
	}else{
		h_hit.y = (map_y * map->cell_size) + map->cell_size;

		int dy = h_hit.y - player->position.y;
		double dx = dy / -tan(TO_RAD(ray_angle));

		h_hit.x = player->position.x + dx;
		step_y = map->cell_size;
	}

	if(ray_angle > 180){
		delta_step_x = map->cell_size / -tan(TO_RAD(ray_angle));
	}else{
		delta_step_x = map->cell_size / tan(TO_RAD(ray_angle));
	}

	bool hit = false;
	if(ray_angle != 0 && ray_angle != 180){
		while(!hit){
			int x = h_hit.x / map->cell_size;
			int y = h_hit.y / map->cell_size;
			if(x > map->w || x < 0 || y > map->h || y < 0){
				map_coords->x = INT_MAX;
				map_coords->y = INT_MAX;
				break;
			}else if(map->values[y * map->w + x] > 0){
				map_coords->x = x;
				map_coords->y = y;
				hit = true;
				// TODO: caculate distance here
			}else{
				h_hit.x += delta_step_x;
				h_hit.y += (double)step_y;
			}
		}
	}

	if(!hit){
		h_hit.x = INT_MAX;
		h_hit.y = INT_MAX;
	}

	return h_hit;
}

static vec2f cast_vertical_intercept(double ray_angle, const Player * player, const Map * map, vec2i * map_coords){
	int step_x;
	double delta_step_y;
	vec2f v_hit;

	// U, R
	int map_x = (int)(player->position.x / map->cell_size);
	if(ray_angle < 90.0 || ray_angle > 270.0){
		v_hit.x = (map_x * map->cell_size) + map->cell_size;
		//NOTE: floating point convesion, careful?
		double dx = v_hit.x - player->position.x;
		double dy = (double)dx * tan(TO_RAD(ray_angle));
		v_hit.y = player->position.y - dy;

		step_x = map->cell_size;
		delta_step_y = -(step_x * tan(TO_RAD(ray_angle)));
	}else{
		v_hit.x = (map_x * map->cell_size) -1;
		double dx = player->position.x - v_hit.x;

		double dy = -((double)dx * tan(TO_RAD(ray_angle)));
		v_hit.y = player->position.y - dy;

		step_x = -map->cell_size;
		delta_step_y = -(step_x * tan(TO_RAD(ray_angle)));
	}

	bool hit = false;
	if(ray_angle != 180 || ray_angle != 90){
		while(!hit){
			int x = v_hit.x / map->cell_size;
			int y = v_hit.y / map->cell_size;

			if(x > map->w || x < 0 || y > map->h || y < 0){
				map_coords->x = INT_MAX;
				map_coords->y = INT_MAX;
				break;
			}
			if(map->values[y * map->w + x] > 0){
				map_coords->x = x;
				map_coords->y = y;
				hit = true;
			}else{
				v_hit.x += (double)step_x;
				v_hit.y += delta_step_y;
			}
		}

	}

	if(!hit){
		v_hit.x = INT_MAX;
		v_hit.y = INT_MAX;
	}

	return v_hit;

}

double real_distance(double angle, const vec2f * a, const vec2f * b){
	double dx = abs(b->x - a->x);
	double dy = abs(b->y - a->y);
	return sqrt(dx*dx + dy*dy);
}

void draw_wall_slice(int y_top, int y_bot, int x, uint32_t color){
	if(y_top < 0)
		y_top = 0;
	if(y_bot >= rctx->proj_plane_w)
		y_bot = rctx->proj_plane_w - 1;

	for(int y = y_top; y < y_bot; y++){
		rctx->fbuffer[y * rctx->proj_plane_w + x] = color;
	}
}

uint32_t * rc_cast(SDL_Renderer * renderer, const Player * player, const Map * map){ 
	//TODO: no need to calculate this here every time move to inie
	double angle_step = player->fov / (double)rctx->proj_plane_w;

	// move the starting ray_angle direction to the leftmost part of the arc
	double ray_angle = player->viewing_angle + (player->fov / 2);

	rc_clear_buffer();
	memset(rctx->hits, 0, sizeof(vec2f) * rctx->proj_plane_w);

	/*Trace a ray for every colum*/
	for(size_t x = 0; x < rctx->proj_plane_w; x++){
		if(ray_angle < 0) ray_angle += 360.0f;
		vec2i map_coords_h, map_coords_v;

		vec2f h_hit = cast_horizontal_intercept(ray_angle, player, map, &map_coords_h);
		vec2f v_hit = cast_vertical_intercept(ray_angle, player, map, &map_coords_v);

		double h_dist = real_distance(ray_angle, &player->position, &h_hit);
		double v_dist = real_distance(ray_angle, &player->position, &v_hit);

		rctx->hits[x] = h_dist < v_dist ? h_hit : v_hit;
		
		vec2i * map_coords = h_dist < v_dist ? &map_coords_h : &map_coords_v;

		double dist_to_wall = MIN(h_dist, v_dist);
		int slice_height = (int)(((double)map->cell_size / dist_to_wall) * player->dist_from_proj_plane);

        int wall_bot = (slice_height * 0.5f) + rctx->proj_plane_center;
	    int wall_top = rctx->proj_plane_center - (slice_height * 0.5f);       

		assert(map_coords->x >= 0     &&
			   map_coords->x < map->w &&
			   map_coords->y >= 0     &&
			   map_coords->y < map->h);

		uint32_t color = colors[map->values[map_coords->y * map->w + map_coords->x]];

		draw_wall_slice(wall_top, wall_bot, x, color);

		ray_angle -= angle_step;
		if(ray_angle > 360.0f) ray_angle -= 360.0f;
	}

	return rctx->fbuffer;
}

void rc_draw_rays(SDL_Renderer * renderer, const Player * player, const Map * map){
	assert(renderer != NULL);

	SDL_RenderSetViewport(renderer, &map->viewport);
	engine_set_color(0xffffffff);

	for(size_t i = 0; i < rctx->proj_plane_w; i++){
		const vec2f * hit = &rctx->hits[i];
		assert(hit != NULL);

		if(hit->x != INT_MAX && hit->y != INT_MAX){
			vec2i player_screen, hit_screen;

			world_2_screen(&player->position, &player_screen, map);
			vec2f hitf;

			hitf.x = hit->x;
			hitf.y = hit->y;

			world_2_screen(&hitf, &hit_screen, map);

			SDL_RenderDrawLine(renderer,
							   player_screen.x,
							   player_screen.y,
							   hit_screen.x,
							   hit_screen.y);
		}
	}
}
