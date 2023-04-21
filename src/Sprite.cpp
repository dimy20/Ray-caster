#include "Sprite.h"
#include <cstring>
#include <iostream>
#include "RC_Core.h"
#include "utils.h"

rc::Sprite::Sprite(const Vec2f& pos, int id, Core * core) : 
	position(pos), texture_id(id), m_core(core){
}

rc::Sprite& rc::Sprite::operator= (const Sprite& other){
	if(this != &other){
		position = other.position;
		texture_id = other.texture_id;
	}
	return *this;
}

void rc::Sprite::draw(const SDL_Rect& dim, double dist_from_player) const {
	int start_x = dim.x;
	int start_y = dim.y;
	int sprite_w = dim.w;
	int sprite_h = dim.h;

	SDL_Surface * texture = m_core->m_resources->get_surface(texture_id);

	auto screen_2_texture_x = static_cast<double>(texture->w) / static_cast<double>(sprite_w);
	auto screen_2_texture_y = static_cast<double>(texture->h) / static_cast<double>(sprite_w);

	for(int x = 0; x < sprite_w; x++){
		int screen_x = x + start_x;

		if(dist_from_player < m_core->m_wall_dists[screen_x]){ // depth test
			if((screen_x < m_core->m_proj_plane_w)){
				for(int y = 0; y < sprite_h; y++){

					int screen_y = start_y + y;
					if(screen_y >= m_core->m_proj_plane_h) continue;

					// TODO: may change to x << 6
					int texture_x = x * screen_2_texture_x;
					int texture_y = y * screen_2_texture_y;

					uint32_t * pixels = reinterpret_cast<uint32_t *>(texture->pixels);
					uint32_t pixel_color = pixels[texture_y * texture->w + texture_x];

					uint32_t color_key;

					if(SDL_GetColorKey(texture, &color_key) < 0){
						m_core->m_fbuffer.set_pixel(screen_x, screen_y, pixel_color);
					}else if(pixel_color != color_key){
						m_core->m_fbuffer.set_pixel(screen_x, screen_y, pixel_color);
					}
				}
			}
		}
	}
}

void rc::Sprite::update(){
	last_dist_to_player = (position - m_core->m_player->position).length();
}
