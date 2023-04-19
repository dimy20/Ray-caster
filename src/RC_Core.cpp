#include "RC_Core.h"
#include <iostream>

#define COLOR_KEY 0x980088ff
#define PROJ_PLANE_W 800
#define PROJ_PLANE_H 600

static bool visited_cell[MAP_MAX_SIZE][MAP_MAX_SIZE];
extern SDL_Texture * sprite_texture;
extern uint32_t temp_map[8 * 8];

uint32_t sprite_pixels[PROJ_PLANE_W * PROJ_PLANE_H];

double rc::Core::perpendicular_distance(double viewing_angle, const Vec2f& p, const Vec2f& hit){
	double dx = hit.x - p.x;
	double dy = p.y - hit.y;
	return (dx * cos(to_rad(viewing_angle))) + (dy * sin(to_rad(viewing_angle)));
}

rc::Core::Core(size_t proj_plane_w, size_t proj_plane_h, double fov){
	m_proj_plane_w = proj_plane_w;
	m_proj_plane_h = proj_plane_h;
	m_proj_plane_center = proj_plane_h / 2;

	m_hits.resize(m_proj_plane_w, Vec2f(0, 0));

	m_angle_step = fov / static_cast<double>(m_proj_plane_w);
	m_fbuffer = Frame_buffer(proj_plane_w, proj_plane_h);
	m_resources = Resources::instance();

	m_player = std::make_unique<Player>(proj_plane_w);
	m_map = std::make_unique<Map>(temp_map, 8, 8);

	m_map->set_sprite(100, 100, 4); // BARREL_SPRITE
}

rc::Core::~Core(){ };

double rc::Core::find_h_intercept(double ray_angle, Vec2f& h_hit, Vec2i& map_coords){
	int step_y;
	double delta_step_x;

	/*Cell grid position for the current player's position.
	 * We actually  this down because we're actually looking for h_hit
	 * x and y positions, not the player's.*/
	int map_y = static_cast<int>(m_player->position.y / m_map->cell_size);

	if(ray_angle > 0.0 && ray_angle < 180.0){
		h_hit.y = map_y * (m_map->cell_size) - 1;

		double dy = (m_player->position.y) - h_hit.y;
		double dx = dy / tan(to_rad(ray_angle));

		h_hit.x = (m_player->position.x) + dx;

		step_y = -m_map->cell_size;
	}else{
		h_hit.y = (map_y * m_map->cell_size) + m_map->cell_size;

		double dy = h_hit.y - (m_player->position.y);
		double dx = dy / -tan(to_rad(ray_angle));

		h_hit.x = (m_player->position.x) + dx;
		step_y = m_map->cell_size;
	}

	delta_step_x = static_cast<double>(m_map->cell_size) / tan(to_rad(ray_angle));
	delta_step_x = (ray_angle > 180.0) ? -delta_step_x : delta_step_x;

	double distance = DBL_MAX;

	bool hit = false;
	if(ray_angle != 0 && ray_angle != 180){
		while(!hit){
			int x = static_cast<int>(h_hit.x) / m_map->cell_size;
			int y = static_cast<int>(h_hit.y) / m_map->cell_size;
			if(x >= m_map->w || x < 0 || y >= m_map->h || y < 0){
				map_coords.x = INT_MAX;
				map_coords.y = INT_MAX;
				break;
			}else if(m_map->at(x, y) & WALL_BIT){

				map_coords.x = x;
				map_coords.y = y;
				hit = true;
				distance = perpendicular_distance(m_player->viewing_angle, m_player->position, h_hit);
			}else{
				visited_cell[y][x] = true;
				h_hit.x += delta_step_x;
				h_hit.y += static_cast<double>(step_y);
			}
		}
	}

	return distance;
}

double rc::Core::find_v_intercept(double ray_angle, Vec2f& v_hit, Vec2i& map_coords){
	int step_x;
	double delta_step_y;

	// U, R
	int map_x = static_cast<int>((m_player->position.x) / m_map->cell_size);
	if(ray_angle < 90.0 || ray_angle > 270.0){
		v_hit.x = (map_x * m_map->cell_size) + m_map->cell_size;
		//NOTE: floating point convesion, careful?
		double dx = v_hit.x - (m_player->position.x);
		double dy = dx * tan(to_rad(ray_angle));
		v_hit.y = (m_player->position.y) - dy;

		step_x = m_map->cell_size;
		delta_step_y = -(step_x * tan(to_rad(ray_angle)));
	}else{
		v_hit.x = (map_x * m_map->cell_size) -1;
		double dx = (m_player->position.x) - v_hit.x;

		double dy = -(dx * tan(to_rad(ray_angle)));
		v_hit.y = (m_player->position.y) - dy;

		step_x = -m_map->cell_size;
		delta_step_y = -(step_x * tan(to_rad(ray_angle)));
	}

	double distance = DBL_MAX;

	bool hit = false;
	if(ray_angle != 180.0 && ray_angle != 90.0){
		while(!hit){
			int x = static_cast<int>(v_hit.x) / m_map->cell_size;
			int y = static_cast<int>(v_hit.y) / m_map->cell_size;
			if(x >= m_map->w || x < 0 || y >= m_map->h || y < 0){
				map_coords.x = INT_MAX;
				map_coords.y = INT_MAX;
				break;
			}else if(m_map->at(x, y) & WALL_BIT){
				map_coords.x = x;
				map_coords.y = y;
				distance = perpendicular_distance(m_player->viewing_angle, m_player->position, v_hit);
				hit = true;
			}else{
				visited_cell[y][x] = true;
				v_hit.x += static_cast<double>(step_x);
				v_hit.y += delta_step_y;
			}
		}
	}

	return distance;
}

void rc::Core::draw_wall_slice(int y_top, int y_bot, int x, uint32_t color){
	if(y_top < 0)
		y_top = 0;

	if(y_bot >= m_proj_plane_h)
		y_bot = m_proj_plane_h;

	for(int y = y_top; y < y_bot; y++){
		assert(x >= 0 && x < m_proj_plane_w);
		assert(y >= 0 && y < m_proj_plane_h);
		m_fbuffer.set_pixel(x, y, color);
	}
}

///*Draws a texture mapped wall slice for the current x value*/
//
void rc::Core::draw_textmapped_wall_slice(int texture_x, int slice_height, int screen_x, SDL_Surface * texture){
	assert(texture != NULL);

	int pixel_y;
	int texture_y;

	size_t texture_size = texture->w;
	int start_y = m_proj_plane_center - (slice_height / 2);

	for(int i = 0; i < slice_height; i++){
		// location of this texture pixel on screen?
		pixel_y = i + start_y;
		if(pixel_y >= 0 && pixel_y < m_proj_plane_h){

			/*Makes the following mapping of values from [0, size] -> [0, column_height]
			*Scaling the original texture to column height*/
			texture_y = ((i * texture_size) / slice_height);
			uint32_t pixel_color = ((uint32_t *)texture->pixels)[texture_y * texture->w + texture_x];

			m_fbuffer.set_pixel(screen_x, pixel_y, pixel_color);

		}
	}
}

///*
// * When drawing a wall slice is finished, We can draw the corresponding  slice for the 
// * previously
// * rendered wall slice. The process in this case will be reversed, instead of casting rays 
// * in world space, finding the distance to wall slicees and corresponding height in screen space,
// * will be finding a position in world space from a position in screen space.
// * (screen_x, y) -> point in world space, we'll map this point to a  texture pixel color
// * and use that color to draw the (screen_x, y) pixel on the screen.
// *  y E [wall_slice_bottom_y, plane_height]
// *
// *  Using similar triangle equation and some trig we find all the values we need.
// * */
void rc::Core::draw_floor_slice(double ray_angle, int screen_x, int wall_bottom_y){
	assert(m_player != NULL);

	Vec2f P;
	double straight_dist_to_P; // the straight distance to the  point P.

	// ray is assumed normalized
	Vec2f ray_dir(cos(to_rad(ray_angle)), -sin(to_rad(ray_angle)));

	double beta = m_player->viewing_angle - ray_angle;
	double cosine_beta = cos(to_rad(beta));

	double pheight_t_distplane = static_cast<double>(m_player->height) * m_player->dist_from_proj_plane;

	for(int y = wall_bottom_y; y < m_proj_plane_h; y++){ // the range mentioned above
		int row_diff = y - m_proj_plane_center;
		// from similar triangle we can find the perpendicular distance from player to P.
		straight_dist_to_P = pheight_t_distplane / static_cast<double>(row_diff);

		/* We can derive this by looking a the scene from a top down perspective.
		   After finding the real distace we can just scale the ray by this value
		   to find p. */

		double real_distance_to_P = straight_dist_to_P / cosine_beta;
		P = m_player->position + (ray_dir * real_distance_to_P);

		int map_x = P.x / m_map->cell_size;
		int map_y = P.y / m_map->cell_size;

		if(map_x >= 0 && map_x < m_map->w && map_y >= 0 && map_y < m_map->h){
			int texture_x = static_cast<int>(P.x) % m_map->cell_size;
			int texture_y = static_cast<int>(P.y) % m_map->cell_size;

			uint32_t cell_data = m_map->at(map_x, map_y);
			if(cell_data & FLOOR_CEIL_BIT){
				int text_index = (int)((cell_data >> 16) & 0xff);

				assert(text_index >= 0);
				SDL_Surface * texture = m_resources->get_surface(text_index);
				assert(texture != NULL);

				assert(texture_x >= 0 && texture_x < texture->w &&
					   texture_y >= 0 && texture_y < texture->h);

				uint32_t pixel_color = ((uint32_t *)texture->pixels)[texture_y * texture->w + texture_x];
				m_fbuffer.set_pixel(screen_x, y, pixel_color);

			}

		}
	}

}
//
///* 
// * This function is symetric to the  slice drawing function.
// * It will draw the corresponding ceiling slice for a given column screen_x.
// * The process of finding the world point P in the ceiling is completley symetric 
// * to process of findig a point P for a  cast. both  and celing drawing could be
// * merged into a single function, however since this raycasting engine will have vertical
// * movement and possible flying, it's better to keep them seperate.
// * */
//
void rc::Core::draw_celing_slice(double ray_angle, int screen_x, int wall_top){
	double straight_dist_to_P;
	double real_dist_to_P;

	double beta = ray_angle - m_player->viewing_angle;
	double cosine_beta = cos(to_rad(beta));

	Vec2f ray_dir(cos(to_rad(ray_angle)), -sin(to_rad(ray_angle)));
	Vec2f P;

	// constant throughout the loop, compute here once.
	double pheight_t_distplane = static_cast<double>(m_player->height) * m_player->dist_from_proj_plane;

	for(int y = wall_top; y >= 0; y--){
		int row_diff = m_proj_plane_center - y;

		straight_dist_to_P = pheight_t_distplane / static_cast<double>(row_diff);
		real_dist_to_P = straight_dist_to_P / cosine_beta;

		P = m_player->position + (ray_dir * real_dist_to_P);

		int map_x = P.x / m_map->cell_size;
		int map_y = P.y / m_map->cell_size;

		if(map_x >= 0 && map_x < m_map->w && map_y >= 0 && map_y < m_map->h){
			int texture_x = static_cast<int>(P.x) % m_map->cell_size;
			int texture_y = static_cast<int>(P.y) % m_map->cell_size;

			uint32_t cell_data = m_map->at(map_x, map_y);
			if(cell_data & FLOOR_CEIL_BIT){
				int ceiling_text_i = (cell_data >> 8) & 0xff;
				assert(ceiling_text_i >= 0);

				SDL_Surface * ceiling_texture = m_resources->get_surface(ceiling_text_i);
				assert(ceiling_texture != NULL);

				assert(texture_x >= 0 && texture_x < ceiling_texture->w &&
					   texture_y >= 0 && texture_y < ceiling_texture->h);

				uint32_t pixel_color = ((uint32_t *)ceiling_texture->pixels)[texture_y * ceiling_texture->w + texture_x];
				m_fbuffer.set_pixel(screen_x, y, pixel_color);
			}
		}
	}
}


rc::Vec2i rc::Core::sprite_world_2_screen(const RC_Sprite& sprite, int columns_per_angle){
	Vec2f sprite_dir = sprite.position - m_player->position;

	double sprite_angle = atan2(-sprite_dir.y, sprite_dir.x) * (180.0f / M_PI);

	if(sprite_angle > 360.0) sprite_angle -= 360.0;
	if(sprite_angle < 0.0) sprite_angle += 360.0;

	/* This is the angle between the sprite directio and the left most ray angle,
	 * we need this angle to find the sprite's screen x center.*/
	double q = (m_player->viewing_angle + (m_player->fov * 0.5f)) - sprite_angle;

	if(first_quadrant(m_player->viewing_angle) && fourth_quadrant(sprite_angle))
		q += 360.0;

	if(fourth_quadrant(m_player->viewing_angle) && first_quadrant(sprite_angle))
		q -= 360.0;

	return Vec2i(static_cast<int>(q * columns_per_angle), m_proj_plane_center);
}

SDL_Rect rc::Core::sprite_screen_dimensions(int index, int screen_x){
	const auto& sprite = m_map->sprites[index];

	Vec2f sprite_dir = sprite.position - m_player->position;
	double dist_to_sprite = sprite_dir.length();

	double A = static_cast<double>(m_map->cell_size) / dist_to_sprite;
	int sprite_h = static_cast<int>(m_player->dist_from_proj_plane * A);

	return {screen_x - (sprite_h >> 1), m_proj_plane_center - (sprite_h >> 1), sprite_h, sprite_h};
}


void rc::Core::render_sprites(SDL_Renderer * renderer){
	const auto& sprite = m_map->sprites[0];

	int columns_per_angle = m_proj_plane_w / m_player->fov; //TODO: constant

	auto screen_coords = sprite_world_2_screen(sprite, columns_per_angle);
	auto sprite_dim = sprite_screen_dimensions(0, screen_coords.x);

	int start_x = sprite_dim.x;
	int start_y = sprite_dim.y;
	int sprite_w = sprite_dim.w;
	int sprite_h = sprite_dim.h;

	const double screen_2_texture = ((double)m_map->cell_size / (double)sprite_w);
	memset(sprite_pixels, 0, sizeof(uint32_t) * (PROJ_PLANE_W * PROJ_PLANE_H));

	for(int x = 0; x < sprite_w; x++){
		int screen_x = x + start_x;

		if((column_in_bounds(screen_x))){
			for(int y = 0; y < sprite_h; y++){
				int screen_y = start_y + y;
				if(!row_in_bounds(screen_y)) continue;
				// TODO: may change to x << 6
				int texture_x = x * screen_2_texture;
				int texture_y = y * screen_2_texture;

				SDL_Surface * surf = m_resources->get_surface(sprite.texture_id);
				uint32_t pixel_color = ((uint32_t *)surf->pixels)[texture_y * surf->w + texture_x];
				if(pixel_color != COLOR_KEY){
					sprite_pixels[screen_y * PROJ_PLANE_W + screen_x] = pixel_color;
				}
			}
		}
	}
}

const uint32_t * rc::Core::render(uint32_t flags){ 
	// move the starting ray_angle direction to the leftmost part of the arc
	double ray_angle = m_player->viewing_angle + (m_player->fov * 0.5);

	m_fbuffer.clear();
	std::fill(m_hits.begin(), m_hits.end(), Vec2f(0, 0));
	memset(visited_cell, 0, sizeof(bool) * MAP_MAX_SIZE * MAP_MAX_SIZE);

	Vec2i map_coords_h, map_coords_v;
	Vec2f h_hit, v_hit;

	// this is constant, so take it out of the loop
	double cell_size_times_dist = static_cast<double>(m_map->cell_size) * m_player->dist_from_proj_plane;

	/*Trace a ray for every colum*/
	for(int x = 0; x < m_proj_plane_w; x++){
		if(ray_angle < 0) ray_angle += 360.0f;

		double h_dist = find_h_intercept(ray_angle, h_hit, map_coords_h);
		double v_dist = find_v_intercept(ray_angle, v_hit, map_coords_v);

		assert(h_dist >= 0 && v_dist >= 0);

		m_hits[x] = h_dist < v_dist ? h_hit: v_hit;

		int texture_x = h_dist < v_dist ? (int)h_hit.x % 64 : (int)v_hit.y % 64;
		
		auto& map_coords = h_dist < v_dist ? map_coords_h : map_coords_v;

		double dist_to_wall = std::min(h_dist, v_dist);
		int slice_height = static_cast<int>(cell_size_times_dist / dist_to_wall);

        int wall_bot = (slice_height * 0.5f) + m_proj_plane_center;
	    int wall_top = m_proj_plane_center - (slice_height * 0.5f);       

		assert(map_coords.x >= 0     &&
			   map_coords.x < m_map->w &&
			   map_coords.y >= 0     &&
			   map_coords.y < m_map->h);


		uint32_t cell_data = m_map->at(map_coords.x, map_coords.y);
		assert(cell_data & WALL_BIT);

		uint32_t cell_index = cell_data >> 8;
		uint32_t color = m_map->colors[cell_index];

		if(flags & DRAW_TEXT_MAPPED_WALLS){
			SDL_Surface * texture = m_resources->get_surface(cell_index);
			if(texture != NULL){
				draw_textmapped_wall_slice(texture_x, slice_height, x, texture);
			}
		}

		if(flags & DRAW_RAW_WALLS){
			draw_wall_slice(wall_top, wall_bot, x, color);
		}

		draw_floor_slice(ray_angle, x, wall_bot);
		draw_celing_slice(ray_angle, x, wall_top);

		ray_angle -= m_angle_step;
		if(ray_angle >= 360.0f) ray_angle -= 360.0f;
	}

	return &m_fbuffer.pixels[0];
}
