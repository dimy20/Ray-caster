
#include "rc_math.h"
#include "RC_Engine.h"

#include "player.h"

rc::Player::Player(int projection_plane_w){
	fov = FOV;

	double half_fov = TO_RAD(fov * 0.5);
	int half_plane_w = projection_plane_w / 2;

	dist_from_proj_plane = static_cast<double>(half_plane_w) / tan(half_fov);

	height = PLAYER_HEIGHT;
	viewing_angle = PLAYER_VIEWING_ANGLE;

	position = Vec2f(96.0, 224.0);

	speed = 80.0f;
	rotation_speed = 120.0f;
}

void rc::Player::draw(const Map * map, Engine * engine){
	auto screen_position = engine->world_2_screen(position);
	SDL_Rect rect = {screen_position.x, screen_position.y, 10, 10};
	SDL_SetRenderDrawColor(engine->renderer(), 0xff, 0xff, 0xff, 0xff); // remove this from here?
	SDL_RenderFillRect(engine->renderer(), &rect);
}

void rc::Player::update(const rc::Engine * engine){
	double delta_time = engine->time.delta_time;

	bool pressed_w = engine->input.keyboard[SDL_SCANCODE_W];
	bool pressed_s = engine->input.keyboard[SDL_SCANCODE_S];

	bool pressed_d = engine->input.keyboard[SDL_SCANCODE_D];
	bool pressed_a = engine->input.keyboard[SDL_SCANCODE_A];

	if(pressed_s || pressed_w){
		Vec2f dir;

		dir.x = cos(TO_RAD(viewing_angle));
		dir.y = -sin(TO_RAD(viewing_angle));

		double _speed = pressed_w ? speed : - speed;
		_speed *= delta_time;

		position += dir * _speed;
	}

	if(pressed_d || pressed_a){
		double _speed = rotation_speed * delta_time;
		viewing_angle += pressed_d ? -_speed : _speed;
	}
}
