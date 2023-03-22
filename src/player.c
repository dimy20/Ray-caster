#include "player.h"

static void world_2_screen(const vec2i * world_pos, vec2i * screen, const Map * map){
	int map_w = map->cell_size * map->w;
	int map_h = map->cell_size * map->h;

	float x_scale = (map->viewport.w) / (float)(map_w);
	float y_scale = (map->viewport.h) / (float)(map_h);

	screen->x = world_pos->x * x_scale;
	screen->y = world_pos->y * y_scale;
}

void player_draw(const Player * player,
				 const Map * map,
				 SDL_Renderer * renderer){

	vec2i screen_position;
	world_2_screen(&player->position, &screen_position, map);

	SDL_Rect rect = {screen_position.x,
					 screen_position.y,
					 5, 5};

	engine_set_color(0xffffffff);
	SDL_RenderFillRect(renderer, &rect);
}

void player_init(Player * player, size_t projection_plane_w){
	player->fov = FOV;
	float half_fov = TO_RAD(player->fov * 0.5f);
	int half_plane_w = projection_plane_w / 2;

	player->dist_from_proj_plane = half_plane_w / tan(half_fov);

	player->height = PLAYER_HEIGHT;
	player->viewing_angle = PLAYER_VIEWING_ANGLE;

	player->position.x = 96;
	player->position.y = 224;
}
