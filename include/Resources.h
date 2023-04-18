#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <unordered_map>

namespace rc{
	struct Resources{
		static Resources * instance(){
			static Resources r;
			return &r;
		};

		SDL_Surface * get_surface(int id) { 
			if(m_surfaces.find(id) == m_surfaces.end()){
				return NULL;
			}else{
				return m_surfaces[id]; 
			}
		};

		void add_surface(int id, SDL_Surface * s) { m_surfaces[id] = s; }

		private:
			std::unordered_map<int, SDL_Surface *> m_surfaces;
	};
}
