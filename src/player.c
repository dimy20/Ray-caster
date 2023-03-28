#include "player.h"
#include "engine.h"
#include "rc_math.h"

void player_draw(const Player * player, const Map * map, SDL_Renderer * renderer){
	vec2i screen_position;
	world_2_screen(map, &player->position, &screen_position);

	SDL_Rect rect = {screen_position.x,
					 screen_position.y,
					 5, 5};

	RC_Engine_set_color(0xffffffff);
	SDL_RenderFillRect(renderer, &rect);
}

void player_init(Player * player, size_t projection_plane_w){
	player->fov = FOV;
	double half_fov = TO_RAD(player->fov * 0.5);
	size_t half_plane_w = projection_plane_w / 2;

	player->dist_from_proj_plane = (double)half_plane_w / tan(half_fov);

	player->height = PLAYER_HEIGHT;
	player->viewing_angle = PLAYER_VIEWING_ANGLE;

	player->position.x = 96;
	player->position.y = 224;

	player->speed = 30.0f;
	player->rotation_speed = 40.0f;
}

void player_update(Player * player){
	double delta_time = RC_Engine_deltatime();

	bool pressed_w = RC_Engine_test_inputkey(SDL_SCANCODE_W);
	bool pressed_s = RC_Engine_test_inputkey(SDL_SCANCODE_S);

	bool pressed_d = RC_Engine_test_inputkey(SDL_SCANCODE_D);
	bool pressed_a = RC_Engine_test_inputkey(SDL_SCANCODE_A);

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
