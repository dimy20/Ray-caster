#include "rc.h"
#include <math.h>
#include <stdbool.h>
#include <limits.h>

static Rc_context * rc_ctx;
static bool initted = false;

void world_2_screen(const vec2i * world_pos, vec2i * screen, const Map * map){
	int map_w = map->cell_size * map->w;
	int map_h = map->cell_size * map->h;

	float x_scale = (map->viewport.w) / (float)(map_w);
	float y_scale = (map->viewport.h) / (float)(map_h);

	screen->x = world_pos->x * x_scale;
	screen->y = world_pos->y * y_scale;
}

void rc_init(Player * player){
	if(initted){
		//TODO: Log error
		return;
	}
	rc_ctx = malloc(sizeof(Rc_context));
	assert(rc_ctx != NULL);

	rc_ctx->projection_plane_w = PROJECTION_PLANE_W;
	rc_ctx->projection_plane_h = PROJECTION_PLANE_H;

	rc_ctx->h_hits = malloc(sizeof(vec2i) * rc_ctx->projection_plane_w);
	assert(rc_ctx->h_hits != NULL);
	memset(rc_ctx->h_hits, 0, sizeof(vec2i) * rc_ctx->projection_plane_w);

	player_init(player, rc_ctx->projection_plane_w);
	initted = true;
}

void rc_quit(){
	assert(rc_ctx != NULL);
	free(rc_ctx->h_hits);
	free(rc_ctx);
}

static vec2i cast_horizontal_intercept(const float ray_angle,
									  const Player * player,
									  const Map * map){
	const int cell_size = map->cell_size;
	vec2i h_hit;

	int step_y;
	int delta_step_x;

	/*Cell grid position for the current player's position.
	 * We actually round this down because we're actually looking for h_hit
	 * x and y positions, not the player's.*/
	int map_y = (int)(player->position.y / map->cell_size);


	// calculate intial horizontal position
	if(ray_angle >= 0 && ray_angle <= 180){ // ray direction is up
		h_hit.y = map_y * (map->cell_size) - 1;

		int dy = player->position.y - h_hit.y;
		int dx = dy / tan(TO_RAD(ray_angle));

		h_hit.x = player->position.x + dx;

		step_y = -map->cell_size;
	}else{
		h_hit.y = (map_y * (map->cell_size)) + cell_size;

		int dy = h_hit.y - player->position.y;
		int dx = dy / tan(TO_RAD(ray_angle));

		h_hit.x = player->position.x + dx;

		step_y = map->cell_size;

	}


	delta_step_x = map->cell_size / tan(TO_RAD(ray_angle));


	bool hit = false;
	if(ray_angle != 0 && ray_angle != 180){
		while(!hit){
			int x = h_hit.x / map->cell_size;
			int y = h_hit.y / map->cell_size;
			if(x > map->w || x < 0 || y > map->h || y < 0){
				break;
			}else if(map->values[y * map->w + x] > 0){
				hit = true;
				// TODO: caculate distance here
			}else{
				h_hit.x += delta_step_x;
				h_hit.y += step_y;
			}
		}
	}

	if(!hit){
		h_hit.x = INT_MAX;
		h_hit.y = INT_MAX;
	}

	return h_hit;
}

void rc_cast(const Player * player, const Map * map){ 

	#ifdef EUCLIDEAN_RAYCASTER
		memset(rc_ctx->h_hits, 0, sizeof(vec2i) * rc_ctx->projection_plane_w);
		float angle_step = player->fov / (float)PROJECTION_PLANE_W;

		// move the starting ray_angle direction to the leftmost part of the
		float ray_angle = player->viewing_angle + (player->fov / 2);

		/*Trace a ray for every colum*/
		for(size_t x = 0; x < rc_ctx->projection_plane_w; x++){
			if(ray_angle < 0) ray_angle += 360.0f;

			vec2i h = cast_horizontal_intercept(ray_angle, player, map);
			rc_ctx->h_hits[x] = h;

			ray_angle -= angle_step;
			// verticals intercepts

			if(ray_angle > 360.0f) ray_angle -= 360.0f;
		}

	#else
		UNIMPLEMENTED;

	#endif

}


void rc_draw_rays(SDL_Renderer * renderer, const Player * player, const Map * map){
	engine_set_color(0xffffffff);
	for(size_t i = 0; i < rc_ctx->projection_plane_w; i++){
		const vec2i * h_hit = &rc_ctx->h_hits[i];
		if(h_hit->x != INT_MAX && h_hit->y != INT_MAX){
			vec2i player_screen, hit_screen;

			world_2_screen(&player->position, &player_screen, map);
			world_2_screen(h_hit, &hit_screen, map);

			SDL_RenderDrawLine(renderer,
							   player_screen.x,
							   player_screen.y,
							   hit_screen.x,
							   hit_screen.y);
		}
	}
}
