#include "player.h"

static void world_2_screen(const vec2f * world_pos, vec2i * screen, const Map * map){
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

	player->speed = 30.0f;
	player->rotation_speed = 40.0f;
}

void player_update(Player * player){
	double delta_time = engine_deltatime();

	bool pressed_w = engine_test_inputkey(SDL_SCANCODE_W);
	bool pressed_s = engine_test_inputkey(SDL_SCANCODE_S);

	bool pressed_d = engine_test_inputkey(SDL_SCANCODE_D);
	bool pressed_a = engine_test_inputkey(SDL_SCANCODE_A);

	if(pressed_s || pressed_w){
		vec2f dir;
		dir.x = cos(TO_RAD(player->viewing_angle));
		dir.y = -sin(TO_RAD(player->viewing_angle));

		double _speed = pressed_w ? player->speed : - player->speed;
		_speed *= delta_time;

		player->position.x += dir.x * _speed;
		player->position.y += dir.y * _speed;
	}

	if(pressed_d || pressed_a){
		vec2f dir;
		dir.x = cos(TO_RAD(player->viewing_angle));
		dir.y = sin(TO_RAD(player->viewing_angle));

		double rotation_speed = player->rotation_speed * delta_time;

		double alpha_cos = cos(TO_RAD(pressed_d ? -rotation_speed : rotation_speed));
		double alpha_sin = sin(TO_RAD(pressed_d ? -rotation_speed : rotation_speed));

		vec2f new_dir;

		new_dir.x = dir.x * alpha_cos - dir.y * alpha_sin;
		new_dir.y = dir.x * alpha_sin + dir.y * alpha_cos;

		double new_angle = (atan2(new_dir.y, new_dir.x) * 180.0) / M_PI;
		player->viewing_angle = new_angle;
	}
}